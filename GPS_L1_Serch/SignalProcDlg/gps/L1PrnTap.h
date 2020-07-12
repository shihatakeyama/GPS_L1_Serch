// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPS L1 衛星情報 PRN
// Source File Nmae : L1PrnTap.h
// 
// 衛星固有の拡散符号(PRN)を提供する。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#ifndef L1PRNTAP_H
#define L1PRNTAP_H

struct L1PrnTap
{
	unsigned short Prn;		// 1-32,193,
	unsigned short PrnKey;
	unsigned char  Enable;
};

class L1StaticPrn
{
public:
	L1StaticPrn();

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// SV → PRN
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static uint16_t svToPrn(uint16_t Sv);
	static uint16_t prnToSv(uint16_t Prn);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// G2レジスタに格納する PrnKeyを戻します
	// 戻り値 ： !0  PrnKey
	//           =0  エラー
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static unsigned short getPrnKey(unsigned Prn);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// 配列の要素番号を衛星情報に変換します。
	// 戻り値 ： !NULL  衛星番号
	//           =NULL  エラー
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static const struct L1PrnTap* getSv(short ElemNum);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// 衛星番号SVを配列の要素番号に変換します。
	// 戻り値 ：>=0  要素番号
	//           <0  エラー
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static short getElemNum(unsigned short Prn);

private:
//	const static L1PrnTap PrnInfo[];
//	static int	SvNum;
};

#endif

