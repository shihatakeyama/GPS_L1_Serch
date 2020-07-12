// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPU_L1_FreqPhase.h
// 
//  キャリアとCAコードの 周波数と位相を格納する
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 1Block内の処理
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
struct GPU_L1_ThreadParam{

	uint32_t CodeFreq;		// CAコード 周波数  (ブロック内で一つ)
	uint32_t CodePhase;		// CAコード 1chip内の位相 ,  コード位相 0～1022
	uint16_t CodeChip;		// CAコード 位相 0～1022

	uint32_t CarrFreq;		// Carr 周波数
	uint32_t CarrPhase;		// Carr 位相
};

// 各スレッドにCAコードとキャリアを割り当て行う。
// Base メンバーは追尾Tracking終了時、終了時の各位相が入ります。
// CPU － GPU 間のI/F
// 1BLOCK内は、SVは1個、CAの位相がスレッドによって異なる。
struct GPU_L1_FreqPhase{

	uint32_t Sv;

	GPU_L1_ThreadParam Base;	// BLOCKの土台 
	GPU_L1_ThreadParam Delta;	// 各スレッドによって異なる。


	void fromCenterCaFreq(uint32_t Clk ,uint32_t CenterFreq ,uint32_t Freq1 ,uint32_t CenterOffset);
	// CaPhase1 : 最大0x100000000 1chip		PhaseN → CenterOffset	  Base + Delta×CenterOffset = Center周波数		or Basement
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

	// サーチ用 キャリア周波数とCA周波数を変更します。
	void fromCenterFreq(uint32_t Clk ,uint32_t CarrCenterFreq ,uint32_t Freq1 ,uint32_t CenterOffset)
	{
		Delta.CarrFreq = clkFreq(Clk , Freq1);
		Base.CarrFreq = clkFreq(Clk ,CarrCenterFreq) - (Delta.CarrFreq * CenterOffset);

		Delta.CodeFreq = clkFreq(Clk ,Freq1)/CC_SCALE;
		Base.CodeFreq = clkFreq(Clk ,(uint32_t)(CODE_FREQ_RES/2)) - (Delta.CodeFreq * CenterOffset);
	}


	// 以下 CUDAが処理
//--	void getCodePhase(uint32_t &Chip ,uint32_t &Phase ,int32_t PhaseDistance) const;
	void getCodePhase(uint16_t &Chip ,uint32_t &Phase ,int32_t PhaseDistance) const;
	uint32_t getCarrPhase(int32_t PhaseDistance) const;

	uint32_t getCodeFreq(int32_t FraqDistance) const;
	uint32_t getCarrFreq(int32_t FraqDistance) const;
};

