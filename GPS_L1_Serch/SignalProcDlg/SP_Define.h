// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// SP_Define.h
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef SP_DEFINE_H
#define SP_DEFINE_H

#include <stdlib.h>
#include <string.h>



#define APPLI_VERSION	100		// �A�v���P�[�V�����o�[�W����		100=1.00



// ���\�[�X resource.h �Œ�`����ƁA�E�C�U�[�h�ɂ���č폜����Ă��܂����߁B
#define WM_COM_RECV_DATA	(WM_USER+1)
#define WM_EDIT_ENTER		(WM_USER+2)


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 32Bit ����Z���s���܂��B 
// Numerator < Denominator (�^����)�̊֌W�𖞂����ĉ������B
// Numeratort��Denominator�̍ŏ�ʃr�b�g�͕K��0�ɂ��ĉ������B
// ����Z����������0x100000000 ���|�����l���߂�l�Ƃ��Ė߂�܂��B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
template<typename X>
inline X decimalDivision(X Numerator ,X Denominator)
{
	X ret = 0;
	int32_t i;

	if((Numerator > Denominator) || ((Numerator & (((X)1) << ((sizeof(Numerator)*8)-1)) ))){		// 0x80000000
//		return 0;
	}

	for(i=32;i>=0;i--){

		if(Numerator >= Denominator){
			ret |= 1 << i;
			Numerator -= Denominator;
		}

		Numerator <<= 1;
	}

	return ret;
}
template<typename X>
inline X clkFreq(X Clk ,X Freq)
{
	return decimalDivision(Freq ,Clk); 
}

#endif
