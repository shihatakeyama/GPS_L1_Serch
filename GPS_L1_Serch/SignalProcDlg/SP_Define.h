// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// SP_Define.h
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef SP_DEFINE_H
#define SP_DEFINE_H

#include <stdlib.h>
#include <string.h>



#define APPLI_VERSION	100		// アプリケーションバージョン		100=1.00



// リソース resource.h で定義すると、ウイザードによって削除されてしまうため。
#define WM_COM_RECV_DATA	(WM_USER+1)
#define WM_EDIT_ENTER		(WM_USER+2)


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 32Bit 割り算を行います。 
// Numerator < Denominator (真分数)の関係を満たして下さい。
// NumeratortとDenominatorの最上位ビットは必ず0にして下さい。
// 割り算した答えに0x100000000 を掛けた値が戻り値として戻ります。
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
