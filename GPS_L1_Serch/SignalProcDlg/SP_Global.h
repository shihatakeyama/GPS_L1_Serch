// **********************************************************************
// SP_Global.h
//
//
// **********************************************************************

#ifndef SP_GLOBAL_H
#define SP_GLOBAL_H


#include "stdafx.h"

#include "L1CaCode.h"

#include "L1Define.h "
#include "GPU_L1_FreqPhase.h"
#include "L1Tracking.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern GLThread					gGLThread;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 通信関連
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//extern LineDataSet				gGraphCsv;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// プロセス関連データ
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//---extern st_CsvParam				gCsvParam;

#define NUM_THREADS		32      // Number of threads per block.
#define MAX_THREADS		32		// メモリ確保で使用する 最大スレッド数

extern std::vector<L1CaCode>	gL1CaCode;
extern L1TrackingCh				gL1TrackingCh[32];

extern int32_t OutData[MAX_THREADS*64*40];
extern int32_t OutiData[MAX_THREADS*64*40];
extern int32_t OutqData[MAX_THREADS*64*40];


// RF設定
// システムクロック
extern uint32_t gSysFreqRes;
// キャリア周波数
extern uint32_t gL1CarrFreqRes;
// コード周波数(ハーフチップ)
extern uint32_t gL1CodeFreqRes;
// サーチ条件
extern GPU_L1_FreqPhase gL1SearchParam;

void L1makeCaCodelist();




#endif // SP_GLOBAL_H

