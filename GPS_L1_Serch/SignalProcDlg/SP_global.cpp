// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// SP_Global.cpp
//
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"

#include <vector>

#include "GnrlDefine.h"
#include "GnrlCharset.h"

#include "L1CaCode.h"

#include "L1Define.h "
#include "GPU_L1_FreqPhase.h"
#include "L1Tracking.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//--LineDataSet				gGraphTest01;
GLThread				gGLThread;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 通信関連
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//--LineDataSet				gGraphCsv;
//--GnrlCom					gCom;
//--ComRecvThread			gComRecvThread;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// プロセス関連データ
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//--st_CsvParam					gCsvParam;




std::vector<L1CaCode>		gL1CaCode;
L1TrackingCh				gL1TrackingCh[32];

int32_t OutData[MAX_THREADS*64*40];
int32_t OutiData[MAX_THREADS*64*40];
int32_t OutqData[MAX_THREADS*64*40];


// RF設定
// システムクロック
uint32_t gSysFreqRes	= (40000000/7);		// gSampleClk
// キャリア周波数
uint32_t gL1CarrFreqRes	= (1405000);
// コード周波数(ハーフチップ)
uint32_t gL1CodeFreqRes	= (2046000);
// サーチ条件
GPU_L1_FreqPhase gL1SearchParam;

