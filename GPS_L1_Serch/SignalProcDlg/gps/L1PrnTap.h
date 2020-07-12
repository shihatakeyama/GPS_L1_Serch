// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPS L1 �q����� PRN
// Source File Nmae : L1PrnTap.h
// 
// �q���ŗL�̊g�U����(PRN)��񋟂���B
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
	// SV �� PRN
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static uint16_t svToPrn(uint16_t Sv);
	static uint16_t prnToSv(uint16_t Prn);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// G2���W�X�^�Ɋi�[���� PrnKey��߂��܂�
	// �߂�l �F !0  PrnKey
	//           =0  �G���[
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static unsigned short getPrnKey(unsigned Prn);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// �z��̗v�f�ԍ����q�����ɕϊ����܂��B
	// �߂�l �F !NULL  �q���ԍ�
	//           =NULL  �G���[
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static const struct L1PrnTap* getSv(short ElemNum);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// �q���ԍ�SV��z��̗v�f�ԍ��ɕϊ����܂��B
	// �߂�l �F>=0  �v�f�ԍ�
	//           <0  �G���[
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	static short getElemNum(unsigned short Prn);

private:
//	const static L1PrnTap PrnInfo[];
//	static int	SvNum;
};

#endif

