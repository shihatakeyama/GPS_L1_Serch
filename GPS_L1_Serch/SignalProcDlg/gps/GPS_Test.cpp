
#if 0
const char i_table[] = {+1 ,+3 ,+3 ,+1 ,-1 ,-3 ,-3 ,-1};	// GP2010 キャリア レプリカ
const char q_table[] = {+3 ,+1 ,-1 ,-3 ,-3 ,-1 ,+1 ,+3};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPSデータを作成
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int32_t GTS_makeTestData(int8_t *OutData ,int32_t Len ,const int8_t *CaCode)
{
	int32_t itr;
	int8_t inca;	

	uint32_t code_chip;
	uint32_t code_phase,code_phase_old;
	uint32_t carr_phase;
	uint32_t code_freq,carr_freq;
	float32_t tp;

	code_chip = 0;
	code_phase_old = code_phase = 0;	// 初期コード位相を設定して下さい。

	code_freq = clkFreq(gSysFreqRes ,(uint32_t)(CODE_FREQ_RES/2));

	carr_phase = 0;
	carr_freq = clkFreq(gSysFreqRes ,gL1CarrFreqRes);

	for(itr=0;itr<Len;itr++){
		
		inca = CaCode[code_chip];
//		inca = 1;
		inca *= i_table[carr_phase>>29];
//		tp = (carr_phase/4294967296.0f) * M_PI*2;
//		inca *= sin(tp) * 3.9f;

		OutData[itr] = inca;

		code_phase += code_freq;
		if(code_phase < code_phase_old)	{
			code_chip++;
			code_chip = code_chip % 1023;
		}
		code_phase_old = code_phase;
		carr_phase += carr_freq;
	}

	return 0;
}
#endif
