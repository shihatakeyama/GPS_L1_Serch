// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPS L1 衛星情報 PRN
// Source File Nmae : L1PrnTap.cpp
// 
// 衛星固有の拡散符号(PRN)を提供する。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"

#include "L1Define.h"
#include "L1prnTap.h"

L1StaticPrn gL1PrnTap;

// SV は0から始まる本システムがつけた衛星番号
const L1PrnTap PrnList[] = {
	 1 ,0x03EC ,0x01 ,	// PRN1 GPS-1
	 2 ,0x03D8 ,0x01 ,
	 3 ,0x03B0 ,0x01 ,
	 4 ,0x0360 ,0x01 ,
	 5 ,0x0096 ,0x01 ,
	 6 ,0x012C ,0x01 ,
	 7 ,0x0196 ,0x01 ,
	 8 ,0x032C ,0x01 ,
	 9 ,0x0258 ,0x01 ,
	10 ,0x0374 ,0x01 ,
	11 ,0x02E8 ,0x01 ,
	12 ,0x03A0 ,0x01 ,
	13 ,0x0340 ,0x01 ,
	14 ,0x0280 ,0x01 ,
	15 ,0x0100 ,0x01 ,
	16 ,0x0200 ,0x01 ,
	17 ,0x0226 ,0x01 ,
	18 ,0x004C ,0x01 ,
	19 ,0x0098 ,0x01 ,
	20 ,0x0130 ,0x01 ,
	21 ,0x0260 ,0x01 ,
	22 ,0x00C0 ,0x01 ,
	23 ,0x00CE ,0x01 ,
	24 ,0x0270 ,0x01 ,
	25 ,0x00E0 ,0x01 ,
	26 ,0x01C0 ,0x01 ,
	27 ,0x0380 ,0x01 ,
	28 ,0x0300 ,0x01 ,
	29 ,0x0056 ,0x01 ,
	30 ,0x00AC ,0x01 ,
	31 ,0x0158 ,0x01 ,
	32 ,0x02B0 ,0x01 ,

	193 ,0x0051 ,0x01 ,	// PRN193 QZSS-1
	194 ,0x0387 ,0x00 ,
	195 ,0x039F ,0x00 ,
	196 ,0x028D ,0x00 ,
	197 ,0x001A ,0x00 ,	// PRN197 QZSS-5
	129 ,0x0055 ,0x01 ,	// PRN129 MTSAT
//	  0 ,0x0316 ,0x00 ,
//	  0 ,0x022C ,0x00 ,
//      0 ,0x00B0 ,0x00 
};
static int MaxSv;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
L1StaticPrn::L1StaticPrn()
{
	int i;

	for(i=0;i<(sizeof(PrnList)/sizeof(L1PrnTap));i++){
		if(PrnList[i].Prn == 0){
			MaxSv = i;
		}
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint16_t L1StaticPrn::svToPrn(uint16_t Sv)
{
	if(Sv >= sizeof(PrnList)/sizeof(L1PrnTap))	return 0;

	return PrnList[Sv].Prn;
}
uint16_t L1StaticPrn::prnToSv(uint16_t Prn)
{
	return getElemNum(Prn);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// G2レジスタに格納する PrnKeyを戻します
// 戻り値 ： !0  PrnKey
//           =0  エラー
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
unsigned short L1StaticPrn::getPrnKey(unsigned Prn)
{
	int i;

	for(i=0;;i++){
		if(PrnList[i].Prn == 0)
			return 0;

		if(PrnList[i].Prn == Prn)
			return PrnList[i].PrnKey;
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 配列の要素番号を衛星情報に変換します。
// 戻り値 ： !NULL  衛星番号
//           =NULL  エラー
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
const struct L1PrnTap* L1StaticPrn::getSv(short ElemNum)
{
	if(ElemNum >= (sizeof(PrnList)/sizeof(L1PrnTap)))
		return NULL;

	return &PrnList[ElemNum];
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 衛星番号SVを配列の要素番号に変換します。
// 戻り値 ：>=0  要素番号
//           <0  エラー
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
short L1StaticPrn::getElemNum(unsigned short Prn)
{
	int i;

	for(i=0;i<MaxSv;i++){
		if(PrnList[i].Prn == 0)
			return -1;

		if(PrnList[i].Prn == Prn)
			return i;
	}

	return -2;
}

