// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPS L1 Shift Register									  
// Source File Nmae : L1CaCode.cpp
// L1 C/A�R�[�h�𐶐�����
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"

#include <stdint.h>
//#include "GnrlInclude.h"
//#include "L1Include.h"
#include "L1PrnTap.h"

#include "L1CaCode.h"




L1ShiftRegister::L1ShiftRegister()
{
	clear();
}
L1ShiftRegister::~L1ShiftRegister()
{}

void L1ShiftRegister::clear()
{
	G1Register	= 0x03FF;
	G2Register	= 0;
	CurrentCode	= 0;
	ChipPhase	= 0;
}

#if 0
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �V�����G�|�b�N�ɓ��B�����ꍇ TRUE ��Ԃ�
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool L1ShiftRegister::isDump()
{
	return (ChipPhase == 0);
}

bool L1ShiftRegister::operator == (const L1ShiftRegister &Arg) const
{
	return (memcmp((void*)&Arg ,(void*)this ,sizeof(*this)) == 0);
}
#endif
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// G1 �� G2 ���W�X�^�V�t�g�s���AC/A�R�[�h�̃J�����g�l��߂��܂��B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int8_t L1ShiftRegister::shiftChip()
{
#if 1
	// C/A Code
	if((G1Register & 0x0001) ^ (G2Register & 0x0001))
	{
		CurrentCode = 1;
	}else 
	{
		CurrentCode = -1;
	}

	// G1 Shift
	if(((G1Register & 0x0080) != 0) ^ ((G1Register & 0x0001) != 0))
		G1Register |= 0x0400;

	G1Register >>= 1;

	// G2 Shift
	if(   ((G2Register & 0x0100) != 0) ^ ((G2Register & 0x0080) != 0)
		^ ((G2Register & 0x0010) != 0) ^ ((G2Register & 0x0004) != 0) 
		^ ((G2Register & 0x0002) != 0) ^ ((G2Register & 0x0001) != 0) )
		G2Register |= 0x0400;

	G2Register >>= 1;

	ChipPhase++;
	if(ChipPhase >= ECHIP_num) // 1023
		ChipPhase = 0;

	return CurrentCode;
#else
	return 1;
#endif
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// G1 �� G2 ���W�X�^�̒l��ݒ肵�܂�  (G1���W�X�^�̊e�r�b�g�͋���1)
// ����:					 
// Uint8 PrnKey : G2 ���W�X�^�ɐݒ肷��l
// �߂�l:
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void L1ShiftRegister::putPrnKey(uint16_t PrnKey)
{
	clear();
	G2Register	= PrnKey & 0x3FF;

//	shiftChip(ECHIP_num);	// 1023 �����operator == �����ƁA�J�����g�������NG�ƂȂ�̂ŁA���������B
}

#if 1
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �C�Ӓ��V�t�g 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int8_t L1ShiftRegister::shiftChip(int16_t Chip)
{
	int32_t i;

	Chip = Chip % ECHIP_num; //1023
	if(Chip <= 0)
		Chip += ECHIP_num;

	for(i=0;i<Chip;i++){
		shiftChip();
	}
	
	return getCurrentCode();
}
#endif



// * * * * * * * * * * * *  L1ShiftRegister / L1CaCode  * * * * * * * * * * * * * * * * * * * * * * *

L1CaCode::L1CaCode(uint16_t PrnKey)
{
	gen(PrnKey);
}

const int8_t &L1CaCode::operator [] (uint16_t ChipNo) const
{
	return m_Chip[ChipNo];
}

void L1CaCode::gen(uint16_t PrnKey){
	int32_t i;
	L1ShiftRegister lsr;

	lsr.putPrnKey(PrnKey);

	for(i=0;i<ECH_chip;i++){
		m_Chip[i] = lsr.shiftChip();
	}	
}

// * * * * * * * * * * * * L1CaCode  * * * * * * * * * * * * * * * * * * * * * * *

void L1makeCaCodelist()
{
	int32_t i;
	const struct L1PrnTap *lpt;

	gL1CaCode.clear();

	i=0;
	while(1){
		lpt = L1StaticPrn::getSv(i);
		if(lpt == nullptr) break;

		gL1CaCode.push_back(L1CaCode(lpt->PrnKey));

		i++;
	}
}


