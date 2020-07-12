// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  GPUでNCOを計算する。
//
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// System includes
#include <stdio.h>
#include <assert.h>

// CUDA runtime
#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
#include <helper_functions.h>
#include <helper_cuda.h>

#include <stdint.h>
#include "SP_Define.h "

#include "L1CaCode.h"

#include "L1Define.h "
//#include "GTS_global.h"
#include "GPU_L1_FreqPhase.h"
#include "L1Tracking.h"

#include "GPU_L1_FreqPhase.hpp"

//--#include "GPU_FIR.h"

#define NUM_THREADS		32      // Number of threads per block.
#define MAX_THREADS		32		// メモリ確保で使用する 最大スレッド数

// L1 CA Code
__constant__ int8_t devCaCode[1023];	// CAコード
uint16_t devCaCodeSv;					// CAコード 衛星番号

// RF設定
// システムクロック
extern uint32_t gSysFreqRes;
// キャリア周波数
extern uint32_t gL1CarrFreqRes;
// コード周波数(ハーフチップ)
extern uint32_t gL1CodeFreqRes;

#if 0
// GP2010 キャリア レプリカ
struct IQSignal IQ_Tble[8] = {
	 IQSignal(+1 ,+3) ,IQSignal(+3 ,+1) ,IQSignal(+3 ,-1) ,IQSignal(+1 ,-3)
	,IQSignal(-1 ,-3) ,IQSignal(-3 ,-1) ,IQSignal(-3 ,+1) ,IQSignal(-1 ,+3)};
#endif

__constant__ char i_table[] = {+1 ,+3 ,+3 ,+1 ,-1 ,-3 ,-3 ,-1};	// GP2010 キャリア レプリカ
__constant__ char q_table[] = {+3 ,+1 ,-1 ,-3 ,-3 ,-1 ,+1 ,+3};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// CA サーチ
//  x:位相
//  y:周波数
// 
// ThreadLen : 1スレッドが処理するサンプリング個数  
//				※位相反転の切り替わりをまたぐ場合があるので、使用注意。
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
__global__ void L1AccumLevel(float *AccumOut ,const uint32_t *SampleIn, int DataLen ,int width, GPU_L1_FreqPhase *Param)
{
	int si,posi;
	short inca;						// charだとマイナスの掛け算ができない？？
	uint16_t dump_co = 0;
	int16_t chip_val;
	int x = threadIdx.x + blockDim .x * blockIdx .x;
	int y =			      blockDim .y * blockIdx .y;

	int dout = x + width * y;		// 自スレッドの担当位置
	int din = 0;					// 入力データ先頭位置	- 0のりしろ部分	(共有メモリに関連して、blockIdx.x間隔での変動になります。)

	uint32_t code_chip;
	uint32_t code_phase,code_phase_old;
	uint32_t carr_phase;
	uint32_t code_freq,carr_freq;

	GPU_L1_FreqPhase &tparam = Param[0];

	int i_sum = 0;
	int q_sum = 0;
	float acum_sum = 0.0f;

    __shared__	int8_t s_data_in[MAX_THREADS*4];

	tparam.getCodePhase(code_chip ,code_phase ,x);
	carr_phase = tparam.getCarrPhase(x);
	code_freq = tparam.getCodeFreq(y);
	carr_freq = tparam.getCarrFreq(y);

	code_phase_old = code_chip;
	chip_val = devCaCode[code_chip];

	if(y==1){
		y=1;
	}
	__syncthreads();

	posi = 0;
	// Tapループ
	while(posi < DataLen){

		si = posi % (MAX_THREADS*4);
		if(si == 0){
			((uint32_t*)s_data_in)[threadIdx.x]	= SampleIn[din + threadIdx.x];
			din		= din + blockDim.x;
		}
		__syncthreads();

		// *** 掛け算 ***
		inca = ((int8_t*)s_data_in)[si];
		inca *= chip_val;
		i_sum += inca * i_table[carr_phase>>29];
		q_sum += inca * q_table[carr_phase>>29];

		// 次サンプルへ
		code_phase += code_freq;
		if(code_phase < code_phase_old)	{
			code_chip++;
			chip_val = devCaCode[code_chip];
			if(code_chip == 1023){
				code_chip = 0;
				dump_co++;
				acum_sum += (float)sqrt((float)((i_sum*i_sum)+(q_sum*q_sum)));
				i_sum = 0;
				q_sum = 0;
			}
		}
		__syncthreads();
		code_phase_old = code_phase;
		carr_phase += carr_freq;

		posi++;
	}

	acum_sum += (float)sqrt((float)((i_sum*i_sum)+(q_sum*q_sum)));

	AccumOut[dout] = acum_sum;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
__global__ void L1Ca_aggregate(uint32_t *AccumOut ,const int32_t *iAccumIn ,const int32_t *qAccumIn , int width)
{
	int x = threadIdx.x + blockDim .x * blockIdx .x;
	int y =			      blockDim .y * blockIdx .y;

	int dout = x + width * y;		// 自スレッドの担当位置

	int i_sum;
	int q_sum;

    __shared__	int32_t s_data_i[MAX_THREADS+2];
    __shared__	int32_t s_data_q[MAX_THREADS+2];

#if 0
	// E,P,L の信号を合計して i×q
	if(threadIdx.x == 0){
		if(x == 0){
			s_data_i[threadIdx.x] = iAccumIn[dout+1023-1];
			s_data_q[threadIdx.x] = qAccumIn[dout+1023-1];
		}else{
			s_data_i[threadIdx.x] = iAccumIn[dout-1];
			s_data_q[threadIdx.x] = qAccumIn[dout-1];
		}
	}
	__syncthreads();
	s_data_i[threadIdx.x+1]	= iAccumIn[dout];
	s_data_q[threadIdx.x+1]	= qAccumIn[dout];

	if(threadIdx.x == (MAX_THREADS-1)){
		s_data_i[threadIdx.x+2]	= iAccumIn[dout+1];
		s_data_q[threadIdx.x+2]	= qAccumIn[dout+1];
	}
	__syncthreads();

	i_sum	= s_data_i[threadIdx.x] + s_data_i[threadIdx.x+1] + s_data_i[threadIdx.x+2];
	q_sum	= s_data_q[threadIdx.x] + s_data_q[threadIdx.x+1] + s_data_q[threadIdx.x+2];
#else
	// Pのみを i×q
	i_sum = iAccumIn[dout];
	q_sum = qAccumIn[dout];
#endif
	AccumOut[dout] = (uint32_t)sqrt((float)(i_sum*i_sum) + (float)(q_sum*q_sum));
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 最大値を探す
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int32_t maxChipPos(const float *InData ,size_t ChipLen)
{
	float max = InData[0];
	size_t  pos = 0,i;

	for(i=1;i<ChipLen;i++){
		if(InData[i]>max){
			max = InData[i];
			pos = i;
		}
	}
	return pos;
}
int32_t maxChipPos(const int32_t *InData ,size_t ChipLen)
{
	int32_t max = InData[0];
	size_t  pos = 0,i;

	for(i=1;i<ChipLen;i++){
		if(InData[i]>max){
			max = InData[i];
			pos = i;
		}
	}
	return pos;
}
int32_t maxChipPos(const int32_t *InData ,size_t FreqN ,size_t ChipLen)
{
	int32_t max = maxChipPos(InData ,ChipLen);
	size_t  pos = 0,i;

	for(i=1;i<ChipLen;i++){
		if(InData[i]>max){
			max = InData[i];
			pos = i;
		}
	}
	return pos;
	

}
extern int cuda_iDivUp(int a, int b);
#if 0
//Round a / b to nearest higher integer value
int cuda_iDivUp(int a, int b)
{
    return (a + (b - 1)) / b;
}
#endif

extern std::vector<L1CaCode>	gL1CaCode;
extern L1TrackingCh				gL1TrackingCh[32];

float OutDataf[MAX_THREADS*64*40];
extern int32_t OutData[MAX_THREADS*64*40];
extern int32_t OutiData[MAX_THREADS*64*40];
extern int32_t OutqData[MAX_THREADS*64*40];
uint32_t gDumpCo[MAX_THREADS*64*40];

#if 1
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  受信レベルのみ見る。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
__host__ int GPU_L1Accumlevel(const uint8_t *InData ,size_t DataLen)
{
	int32_t ch;
	int32_t ack,tmp2,max;
	uint32_t *dev_in=0;
	float *dev_out=0;
	int32_t *dev_iout=0;
	int32_t *dev_qout=0;

    GPU_L1_FreqPhase *dev_caparam = 0;
	GPU_L1_FreqPhase caparam,caparambk;
    cudaError_t cudaStatus;
	int32_t width	= 2048;
	int32_t height	= 40;
	GPU_L1_ThreadParam check;

	caparam.Sv = 1;

	memset(gL1TrackingCh ,0 ,sizeof(gL1TrackingCh));

#if 1
	caparam.fromCenterFreq(gSysFreqRes ,gL1CarrFreqRes ,500 ,20);
//--	caparam.fromCenterCaPhase(gSysFreqRes ,1023/2+1 ,0x00000000 ,0x80000000 ,0);
//--	caparam.fromCenterCarrPhase(gSysFreqRes ,0 ,500 ,40);
	caparam.fromBaseCodePhase(gSysFreqRes ,1023-10 ,0 ,0x80000000 ,0);
//--	check.CodeFreq = caparam. Base.CodeFreq + (caparam.Delta.CodeFreq * 20);
//--	check.CarrFreq = caparam.Base.CarrFreq + (caparam.Delta.CarrFreq * 20);// caparam.getCarrFreq(20);

	 ack = caparam.getCarrFreq(20);
	 tmp2 = caparam.getCodeFreq(20);

#else

	caparam.Base.CodeChip = 1023 - 10;
	caparam.Base.CodePhase = 0x00000000;		// 0
	caparam.Base.CodeFreq = clkFreq(SYS_FREQ_RES ,CODE_FREQ_RES/2);
	caparam.Base.CarrPhase = 0x00000000;		// 0
	caparam.Base.CarrFreq = clkFreq(SYS_FREQ_RES ,CARR_FREQ_RES );

	caparam.Delta.CodeChip = 0;
	caparam.Delta.CodePhase = 0x80000000;		// 0.5 chip
	caparam.Delta.CodeFreq = 0;
	caparam.Delta.CarrPhase = 0;
	caparam.Delta.CarrFreq = clkFreq(SYS_FREQ_RES ,500);
#endif

	// 入力データ メモリ
 	cudaStatus = cudaMalloc((void**)&dev_in, DataLen * sizeof(uint8_t));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	// 出力データ メモリ
	cudaStatus = cudaMalloc((void**)&dev_out, width*height*sizeof(float)+8);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	// パラメータ メモリ
	cudaStatus = cudaMalloc((void**)&dev_caparam, sizeof(GPU_L1_FreqPhase));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	// 入力データをコピー
	cudaMemcpy((void*)dev_in ,(void*)InData ,DataLen * sizeof(uint8_t) ,cudaMemcpyHostToDevice);

	// パラメータ コピー
	cudaMemcpy((void*)dev_caparam ,(void*)&caparam ,sizeof(GPU_L1_FreqPhase) ,cudaMemcpyHostToDevice);

	dim3 block(NUM_THREADS, 1, 1);
    dim3 grid(cuda_iDivUp(width ,block.x) ,cuda_iDivUp(height ,block.y), 1);

	for(ch=0;ch<32;ch++){
		L1TrackingCh &tack = gL1TrackingCh[ch];

		// C/Aコードをコピー
		cudaStatus = cudaMemcpyToSymbol(devCaCode ,(const int8_t*)(&gL1CaCode[ch][0]) ,1023 * sizeof(int8_t));
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMemcpyToSymbol failed!");
			goto Error;
		}

		// Launch a kernel on the GPU with one thread for each element.
		L1AccumLevel<<<grid, block>>>(dev_out ,dev_in ,DataLen ,width ,dev_caparam);


		// 相関結果コピー
		cudaMemcpy((void*)OutDataf ,(void*)dev_out ,width*height*sizeof(float) ,cudaMemcpyDeviceToHost);
		ack = maxChipPos(OutDataf ,2048*40);
		
		tack.CodePhase		= ack % 2048;
		tack.CarrDcoAcquire	= ack / 2048;	// caparam.getCarrFreq(ack / 2048);

		tack.Sv = ch;
		tack.CdLI = OutDataf[ack];

//		memcpy(gL1TrackingCh[ch].iFF ,&OutiData[ack-16] ,NUM_THREADS * sizeof(float));
//		memcpy(gL1TrackingCh[ch].qFF ,&OutqData[ack-16] ,NUM_THREADS * sizeof(float));

		// パラメータ コピー
//		cudaMemcpy((void*)&caparambk ,(void*)dev_caparam ,sizeof(GPU_L1_FreqPhase) ,cudaMemcpyDeviceToHost);
	}

Error:
	cudaFree(dev_in);
	cudaFree(dev_out);
    cudaFree(dev_caparam);

	return 0;
}
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// CA 相関 GP2010参照
//
//  x:周波数、位相
//  y:GPU_L1_FreqPhase[] 1要素使用
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
__global__ void L1Ca_corr(int32_t *DumpI ,int32_t *DumpQ ,uint32_t *DumpCo ,int32_t *AccumI ,int32_t *AccumQ ,const uint32_t *SampleIn, size_t DataLen ,int width, GPU_L1_FreqPhase *Param)
{
	int si,posi;
	short inca;						// charだとマイナスの掛け算ができない？？
	uint16_t dump_co = 0;
	int16_t chip_val;
	int x = threadIdx.x + blockDim .x * blockIdx .x;
	int y =			      blockDim .y * blockIdx .y;

	int dout = x + width * y;		// 自スレッドの担当位置
	int din = 0;					// 入力データ先頭位置	- 0のりしろ部分	(共有メモリに関連して、blockIdx.x間隔での変動になります。)

	uint32_t code_chip;
	uint32_t code_phase,code_phase_old;
	uint32_t carr_phase;
	uint32_t code_freq,carr_freq;

	GPU_L1_FreqPhase &tparam = Param[y];

	int i_sum = AccumI[dout];
	int q_sum = AccumQ[dout];

    __shared__	int8_t s_data_in[MAX_THREADS*4];

	tparam.getCodePhase(code_chip ,code_phase ,x);
	carr_phase = tparam.getCarrPhase(y);
	code_freq = tparam.getCodeFreq(y);
	carr_freq = tparam.getCarrFreq(y);

	code_phase_old = code_chip;
	chip_val = devCaCode[code_chip];

	__syncthreads();

	posi = 0;
	// Tapループ
	while(posi < DataLen){

		si = posi % (MAX_THREADS*4);
		if(si == 0){
			((uint32_t*)s_data_in)[threadIdx.x]	= SampleIn[din + threadIdx.x];
			din		= din + blockDim.x;
		}
		__syncthreads();

		// *** 掛け算 ***
		inca = ((int8_t*)s_data_in)[si];
		inca *= chip_val;
		i_sum += inca * i_table[carr_phase>>29];
		q_sum += inca * q_table[carr_phase>>29];

		// 次サンプルへ
		code_phase += code_freq;
		if(code_phase < code_phase_old)	{
			code_chip++;
			chip_val = devCaCode[code_chip];
			if(code_chip == 1023){
				code_chip = 0;
				dump_co++;
//--				if((i_sum==0) && (q_sum==0)){
//--					i_sum = 0;
//--				}
				DumpI[dout] = i_sum;
				DumpQ[dout] = q_sum;
				i_sum = 0;
				q_sum = 0;
//						if(threadIdx.x == 15){}	// 追尾処理
			}
		}
		__syncthreads();
		code_phase_old = code_phase;
		carr_phase += carr_freq;

		posi++;
	}

	DumpCo[dout] = dump_co;

	AccumI[dout] = i_sum;
	AccumQ[dout] = q_sum;

	if(threadIdx.x == 0){
		tparam.Base.CodeChip = code_chip;
		tparam.Base.CodePhase = code_phase;
		tparam.Base.CarrPhase = carr_phase;
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  1msサーチ
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
__host__ int GPU_L1Search_gpu2(float *line1 ,const uint8_t *InData ,size_t DataLen)
{
	int32_t ch,i;
	int32_t tmp,tmp2,max;
	uint32_t *dev_in=0;
	uint32_t *dev_out=0;
	int32_t *dev_iout=0;
	int32_t *dev_qout=0;
	uint32_t *dev_dumpco=0;
	int32_t *dev_iaccum=0;
	int32_t *dev_qaccum=0;

    GPU_L1_FreqPhase *dev_caparam = 0;
	GPU_L1_FreqPhase caparam[40],caparambk;
    cudaError_t cudaStatus;
	int32_t width	= 2048;
	int32_t height	= 40;

//	caparam.Sv = 1;

	memset(gL1TrackingCh ,0 ,sizeof(gL1TrackingCh));

#if 0
	caparam.fromCenterFreq(SYS_FREQ_RES ,CARR_FREQ_RES ,500 ,20);
	caparam.fromBaseCodePhase(SYS_FREQ_RES ,1023-10 ,0 ,0x80000000 ,0);
#else
	for(i=0;i<40;i++){
		GPU_L1_FreqPhase &para = caparam[i];
		para.Base.CodeChip = 1023 - 10;
		para.Base.CodePhase= 0x00000000;		// 0
		para.Base.CodeFreq = clkFreq<uint32_t>(gSysFreqRes ,gL1CodeFreqRes/2+(((i-20)*500)/CC_SCALE));
///		para.Base.CodeFreq = clkFreq(SYS_FREQ_RES ,CODE_FREQ_RES/2);
		para.Base.CarrPhase= 0x00000000;		// 0
		para.Base.CarrFreq = clkFreq<uint32_t>(gSysFreqRes ,gL1CarrFreqRes+((i-20)*500));
///		para.Base.CarrFreq = clkFreq(SYS_FREQ_RES ,CARR_FREQ_RES);

		para.Delta.CodeChip	= 0;
		para.Delta.CodePhase= 0x80000000;		// 0.5 chip
		para.Delta.CodeFreq = 0;
		para.Delta.CarrPhase= 0;
		para.Delta.CarrFreq = 0;	// clkFreq(SYS_FREQ_RES ,500);
	}

///	DataLen = 5714+3;
#endif

	// 入力データ メモリ
 	cudaStatus = cudaMalloc((void**)&dev_in, DataLen * sizeof(uint8_t));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	// 出力データ メモリ
	cudaStatus = cudaMalloc((void**)&dev_out, width*height*sizeof(uint32_t));	// ****
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }
	// DUMP I メモリ
	cudaStatus = cudaMalloc((void**)&dev_iout, width*height*sizeof(int32_t));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }
	// DUMP Q メモリ
	cudaStatus = cudaMalloc((void**)&dev_qout, width*height*sizeof(int32_t));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }
	// DUMP カウンタ メモリ
	cudaStatus = cudaMalloc((void**)&dev_dumpco, width*height*sizeof(int32_t));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	// ACCUM I メモリ
	cudaStatus = cudaMalloc((void**)&dev_iaccum, width*height*sizeof(int32_t));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }
	// ACCUM Q メモリ
	cudaStatus = cudaMalloc((void**)&dev_qaccum, width*height*sizeof(int32_t));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	// パラメータ メモリ
	cudaStatus = cudaMalloc((void**)&dev_caparam, height*sizeof(GPU_L1_FreqPhase));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	// 入力データをコピー
	cudaMemcpy((void*)dev_in ,(void*)InData ,DataLen * sizeof(uint8_t) ,cudaMemcpyHostToDevice);

	// パラメータ コピー
	cudaMemcpy((void*)dev_caparam ,(void*)&caparam ,height*sizeof(GPU_L1_FreqPhase) ,cudaMemcpyHostToDevice);

	dim3 block(NUM_THREADS, 1, 1);
    dim3 grid(cuda_iDivUp(width ,block.x) ,cuda_iDivUp(height ,block.y), 1);
	
	for(ch=0;ch<32;ch++){
		max = 0;

		// C/Aコードをコピー
		cudaStatus = cudaMemcpyToSymbol(devCaCode ,(const int8_t*)(&gL1CaCode[ch][0]) ,1023 * sizeof(int8_t));
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMemcpyToSymbol failed!");
			goto Error;
		}

		cudaMemset(dev_iaccum , 0 ,width*height*sizeof(int32_t));
		cudaMemset(dev_qaccum , 0 ,width*height*sizeof(int32_t));

		// Launch a kernel on the GPU with one thread for each element.
		L1Ca_corr<<<grid, block>>>(dev_iout ,dev_qout ,dev_dumpco ,dev_iaccum ,dev_qaccum ,dev_in , DataLen ,width ,dev_caparam);

		L1Ca_aggregate<<<grid, block>>>(dev_out ,dev_iout ,dev_qout ,width);	// i,qを合成

		// 相関結果コピー
		cudaMemcpy((void*)OutiData ,(void*)dev_iout ,width*height*sizeof(int32_t) ,cudaMemcpyDeviceToHost);
		cudaMemcpy((void*)OutqData ,(void*)dev_qout ,width*height*sizeof(int32_t) ,cudaMemcpyDeviceToHost);
		cudaMemcpy((void*)gDumpCo ,dev_dumpco		,width*height*sizeof(int32_t) ,cudaMemcpyDeviceToHost);
		cudaMemcpy((void*)OutData ,(void*)dev_out	,width*height*sizeof(int32_t) ,cudaMemcpyDeviceToHost);
		tmp = maxChipPos(OutData ,2048*40);
		if(max<OutData[tmp]){
			max = OutData[tmp];
		}

		gL1TrackingCh[ch].Sv = ch;
		gL1TrackingCh[ch].CdLI = max;

		memcpy(gL1TrackingCh[ch].iFF ,&OutiData[tmp-16] ,NUM_THREADS * sizeof(float));
		memcpy(gL1TrackingCh[ch].qFF ,&OutqData[tmp-16] ,NUM_THREADS * sizeof(float));

		// パラメータ コピー
		cudaMemcpy((void*)&caparambk ,(void*)dev_caparam ,sizeof(GPU_L1_FreqPhase) ,cudaMemcpyDeviceToHost);
	}

//--	for(i=0;i<2048;i++){
//--		line1[i] = OutData[i+tmp-1000];
//--	}

Error:
	cudaFree(dev_in);
	cudaFree(dev_out);
	cudaFree(dev_iout);
	cudaFree(dev_qout);
	cudaFree(dev_dumpco);
	cudaFree(dev_iaccum);
	cudaFree(dev_qaccum);
    cudaFree(dev_caparam);

	return 0;
}