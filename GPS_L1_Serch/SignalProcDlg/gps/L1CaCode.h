// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// L1CaCode.h
//
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *



#ifndef L1CACODE_H
#define L1CACODE_H


class L1ShiftRegister
{
public:
	enum EChip
	{
		ECHIP_num = 1023
	};

	L1ShiftRegister();
	~L1ShiftRegister();

	void clear();
	void putPrnKey(uint16_t PrnKey);

	uint16_t getG1Register()	const {return G1Register;}
	uint16_t getG2Register()	const {return G2Register;}
	uint16_t getPrnKey()	const {return G2Register;}
	uint16_t getChipPhase()	const {return ChipPhase; }
	int8_t  getCurrentCode()	const {return CurrentCode;}
//--	Bool isDump();			// 新しいエポックに到達した場合 TRUE を返す

//--	Bool operator == (const L1ShiftRegister &Arg) const;
//--	Bool operator != (const L1ShiftRegister &Arg) const {return ! operator == (Arg); }

	int8_t shiftChip();		// Chipを1chipシフトします。　シフト後のC/Aコードを戻す。
	int8_t shiftChip(int16_t Chip);

private:
	uint16_t G1Register;
	uint16_t G2Register;
	int8_t  CurrentCode;
	uint16_t ChipPhase;	// 0～1022
};



class L1CaCode
{
public:
	enum EChip{
		ECH_chip =	1023 // MAX_L1_CA_CODE
	};

	L1CaCode(uint16_t PrnKey);

	const int8_t &operator [] (uint16_t ChipNo) const;

	void gen(uint16_t PrnKey);

	int8_t m_Chip[ECH_chip];
};



#endif

