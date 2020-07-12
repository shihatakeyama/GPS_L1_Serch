// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPU_L1_FreqPhase.h
// 
//  �L�����A��CA�R�[�h�� ���g���ƈʑ����i�[����
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 1Block���̏���
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
struct GPU_L1_ThreadParam{

	uint32_t CodeFreq;		// CA�R�[�h ���g��  (�u���b�N���ň��)
	uint32_t CodePhase;		// CA�R�[�h 1chip���̈ʑ� ,  �R�[�h�ʑ� 0�`1022
	uint16_t CodeChip;		// CA�R�[�h �ʑ� 0�`1022

	uint32_t CarrFreq;		// Carr ���g��
	uint32_t CarrPhase;		// Carr �ʑ�
};

// �e�X���b�h��CA�R�[�h�ƃL�����A�����蓖�čs���B
// Base �����o�[�͒ǔ�Tracking�I�����A�I�����̊e�ʑ�������܂��B
// CPU �| GPU �Ԃ�I/F
// 1BLOCK���́ASV��1�ACA�̈ʑ����X���b�h�ɂ���ĈقȂ�B
struct GPU_L1_FreqPhase{

	uint32_t Sv;

	GPU_L1_ThreadParam Base;	// BLOCK�̓y�� 
	GPU_L1_ThreadParam Delta;	// �e�X���b�h�ɂ���ĈقȂ�B


	void fromCenterCaFreq(uint32_t Clk ,uint32_t CenterFreq ,uint32_t Freq1 ,uint32_t CenterOffset);
	// CaPhase1 : �ő�0x100000000 1chip		PhaseN �� CenterOffset	  Base + Delta�~CenterOffset = Center���g��		or Basement
	void fromCenterCaPhase(uint32_t Clk ,uint32_t CenterCaChip ,uint32_t CenterChipPhase ,uint32_t Phase1 ,uint32_t CenterOffset);

#if 1
	void fromBaseCodePhase(uint32_t Clk ,uint32_t BaseCodeChip ,uint32_t BaseCodePhase ,uint32_t CaPhase1 ,uint32_t CenterOffset){
		Base.CodeChip = BaseCodeChip;
		Base.CodePhase= BaseCodePhase;
		Delta.CodeChip = 0;
		Delta.CodePhase = CaPhase1;
	}
#endif
	void fromCenterDcFreq(uint32_t Clk ,uint32_t CenterFreq ,uint32_t Freq1 ,uint32_t CenterOffset){
		Base.CarrFreq = clkFreq(Clk ,CenterFreq - (Freq1 * CenterOffset));
		Delta.CarrFreq = clkFreq(Clk , Freq1);
	}
	void fromCenterCarrPhase(uint32_t Clk ,uint32_t CenterPhase ,uint32_t Phase1 ,uint32_t CenterOffset){
		Base.CarrPhase = CenterPhase - (Phase1 * CenterOffset);
		Delta.CarrPhase = Phase1;
	}

	// �T�[�`�p �L�����A���g����CA���g����ύX���܂��B
	void fromCenterFreq(uint32_t Clk ,uint32_t CarrCenterFreq ,uint32_t Freq1 ,uint32_t CenterOffset)
	{
		Delta.CarrFreq = clkFreq(Clk , Freq1);
		Base.CarrFreq = clkFreq(Clk ,CarrCenterFreq) - (Delta.CarrFreq * CenterOffset);

		Delta.CodeFreq = clkFreq(Clk ,Freq1)/CC_SCALE;
		Base.CodeFreq = clkFreq(Clk ,(uint32_t)(CODE_FREQ_RES/2)) - (Delta.CodeFreq * CenterOffset);
	}


	// �ȉ� CUDA������
//--	void getCodePhase(uint32_t &Chip ,uint32_t &Phase ,int32_t PhaseDistance) const;
	void getCodePhase(uint16_t &Chip ,uint32_t &Phase ,int32_t PhaseDistance) const;
	uint32_t getCarrPhase(int32_t PhaseDistance) const;

	uint32_t getCodeFreq(int32_t FraqDistance) const;
	uint32_t getCarrFreq(int32_t FraqDistance) const;
};

