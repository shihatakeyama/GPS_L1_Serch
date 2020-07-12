// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPU_L1_FreqPhase.hpp
// 
// ƒLƒƒƒŠƒA‚ÆCAƒR[ƒh‚Ì ü”g”‚ÆˆÊ‘Š‚ğŠi”[‚·‚é
// 
// DEVICE F CUDA
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#if 1
__host__ void GPU_L1_FreqPhase::fromCenterCaFreq(uint32_t Clk ,uint32_t CenterFreq ,uint32_t Freq1 ,uint32_t CenterOffset){
	Base.CodeFreq = clkFreq(Clk ,CenterFreq - (Freq1 * CenterOffset));
	Delta.CodeFreq = clkFreq(Clk ,Freq1);
}

// CaPhase1 : Å‘å0x100000000 1chip		PhaseN ¨ CenterOffset	  Base + Delta~CenterOffset = Centerü”g”		or Basement
__host__ void GPU_L1_FreqPhase::fromCenterCaPhase(uint32_t Clk ,uint32_t CenterCaChip ,uint32_t CenterChipPhase ,uint32_t Phase1 ,uint32_t CenterOffset){
	uint64_t caphase = ((((uint64_t)CenterCaChip)<<32) + CenterChipPhase);
	uint64_t caphase2 = ((uint64_t)Phase1 *CenterOffset);
	while(caphase < caphase2){
		caphase += 4393751543808l;	// 1023chip;
	}
	caphase -= caphase2;
	Base.CodeChip = (uint32_t)(caphase>>32);
	Base.CodePhase= (uint32_t)caphase;
	Delta.CodeChip = 0;
	Delta.CodePhase = Phase1;
}
#endif


__device__ __host__ void GPU_L1_FreqPhase::getCodePhase(uint16_t &Chip ,uint32_t &Phase ,int32_t PhaseDistance) const
{
	uint64_t codephase = (((uint64_t)Base.CodeChip) << 32) + (uint64_t)Base.CodePhase;	//  + ((uint64_t)Delta.CodeChip*PhaseDistance)

	codephase += (((uint64_t)(Delta.CodePhase)) * PhaseDistance);
	Phase = (uint32_t)codephase;
	Chip = ((uint32_t)(codephase>>32)) % 1023;
}

__device__ __host__ uint32_t GPU_L1_FreqPhase::getCarrPhase(int32_t PhaseDistance) const
{
	return Base.CarrPhase + (Delta.CarrPhase * PhaseDistance);
}

__device__ __host__ uint32_t GPU_L1_FreqPhase::getCodeFreq(int32_t FraqDistance) const
{
	return Base.CodeFreq + (Delta.CodeFreq * FraqDistance);
}

__device__ __host__ uint32_t GPU_L1_FreqPhase::getCarrFreq(int32_t FraqDistance) const
{
	return Base.CarrFreq + (Delta.CarrFreq * FraqDistance);
}


