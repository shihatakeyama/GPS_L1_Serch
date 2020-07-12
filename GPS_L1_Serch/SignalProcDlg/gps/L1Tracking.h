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


#define MAX_CORR_TRACKING	32		// �ǔ����̎��̑��֊�


// �R�[�h�ʑ��A�L�����A�ʑ��́A���S(Prompt)�Ƃ��܂��B

struct L1TrackingCh{

	uint16_t	Sv;						// �q���ԍ�

	uint32_t	CdLI;

	// **** �t�@�C�� BEGIN ****
	size_t		SampleBase;				// A/D�T���v�����O�f�[�^�I�t�Z�b�g
	// **** �t�@�C�� END ****

	// **** GPU BEGIN ****
	int32_t	iFF[MAX_CORR_TRACKING];		// GPU�ŐώZ����I�l
	int32_t	qFF[MAX_CORR_TRACKING];		// GPU�ŐώZ����Q�l


#if 0
	uint32_t	CodeChip;					// SampleBase�̎��̈ʑ�
	uint32_t	CodePhase;
	uint32_t	CarrPhase;
	// **** GPU END ****

	uint32_t	CARRDCO;
	uint32_t	CODEDCO;
#endif
	uint32_t	CarrDcoAcquire;		// �L�����ADCO�擾�����l	��CARRDCOAcquire
	uint32_t	CodeDcoAcquire;		// CA�R�[�hDCO�擾�����l	��CODEDCOAcquire
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

