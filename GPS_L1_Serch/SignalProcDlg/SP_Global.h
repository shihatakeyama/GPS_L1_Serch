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
// �ʐM�֘A
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//extern LineDataSet				gGraphCsv;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �v���Z�X�֘A�f�[�^
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//---extern st_CsvParam				gCsvParam;

#define NUM_THREADS		32      // Number of threads per block.
#define MAX_THREADS		32		// �������m�ۂŎg�p���� �ő�X���b�h��

extern std::vector<L1CaCode>	gL1CaCode;
extern L1TrackingCh				gL1TrackingCh[32];

extern int32_t OutData[MAX_THREADS*64*40];
extern int32_t OutiData[MAX_THREADS*64*40];
extern int32_t OutqData[MAX_THREADS*64*40];


// RF�ݒ�
// �V�X�e���N���b�N
extern uint32_t gSysFreqRes;
// �L�����A���g��
extern uint32_t gL1CarrFreqRes;
// �R�[�h���g��(�n�[�t�`�b�v)
extern uint32_t gL1CodeFreqRes;
// �T�[�`����
extern GPU_L1_FreqPhase gL1SearchParam;

void L1makeCaCodelist();




#endif // SP_GLOBAL_H

