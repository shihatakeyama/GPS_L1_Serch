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
// �ʐM�֘A
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//--LineDataSet				gGraphCsv;
//--GnrlCom					gCom;
//--ComRecvThread			gComRecvThread;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �v���Z�X�֘A�f�[�^
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//--st_CsvParam					gCsvParam;




std::vector<L1CaCode>		gL1CaCode;
L1TrackingCh				gL1TrackingCh[32];

int32_t OutData[MAX_THREADS*64*40];
int32_t OutiData[MAX_THREADS*64*40];
int32_t OutqData[MAX_THREADS*64*40];


// RF�ݒ�
// �V�X�e���N���b�N
uint32_t gSysFreqRes	= (40000000/7);		// gSampleClk
// �L�����A���g��
uint32_t gL1CarrFreqRes	= (1405000);
// �R�[�h���g��(�n�[�t�`�b�v)
uint32_t gL1CodeFreqRes	= (2046000);
// �T�[�`����
GPU_L1_FreqPhase gL1SearchParam;

