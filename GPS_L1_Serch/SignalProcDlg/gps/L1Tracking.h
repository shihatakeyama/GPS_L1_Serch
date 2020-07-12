// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// L1Tracking.h
// 
// 
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#ifndef L1TRACKING_H
#define L1TRACKING_H

#include <stdint.h>
#include <vector>

#include "GPU_L1_FreqPhase.h"


#define MAX_CORR_TRACKING	32		// 追尾中の時の相関器


// コード位相、キャリア位相は、中心(Prompt)とします。

struct L1TrackingCh{

	uint16_t	Sv;						// 衛星番号

	uint32_t	CdLI;

	// **** ファイル BEGIN ****
	size_t		SampleBase;				// A/Dサンプリングデータオフセット
	// **** ファイル END ****

	// **** GPU BEGIN ****
	int32_t	iFF[MAX_CORR_TRACKING];		// GPUで積算中のI値
	int32_t	qFF[MAX_CORR_TRACKING];		// GPUで積算中のQ値


#if 0
	uint32_t	CodeChip;					// SampleBaseの時の位相
	uint32_t	CodePhase;
	uint32_t	CarrPhase;
	// **** GPU END ****

	uint32_t	CARRDCO;
	uint32_t	CODEDCO;
#endif
	uint32_t	CarrDcoAcquire;		// キャリアDCO取得した値	旧CARRDCOAcquire
	uint32_t	CodeDcoAcquire;		// CAコードDCO取得した値	旧CODEDCOAcquire
	uint32_t	CarrPhase;
	uint32_t	CodePhase;
	uint16_t	CodeChip;
#if 0
    unsigned long Status;			// Bit-field indicating channel lock status.

	short _1MS_EPOCH_SLEW;          // Error in 1-ms epoch counter.
    short _20MS_EPOCH_SLEW;         // Error in 20-ms epoch counter.

	long EML;
	short NEML;
	long IM1;
	long QM1;
	long wdot_c;
#endif

} ;


// GPU 
struct GPU_L1Channel{

//	GPU_L1_FreqPhase FreqPhase;



};



#endif

