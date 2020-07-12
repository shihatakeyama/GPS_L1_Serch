// *********************************************************************************
//  GnrlCharset.h
//
// *********************************************************************************


#ifndef GNRL_NCHAR_H
#define GNRL_NCHAR_H



//  �v���W�F�N�g�Őݒ肷�镶���̈������z������B
#if 0
	
	Nchar	char			Nchar;
	#define Nstrlen(a)		strlen(a)

#else
	// �v���W�F�N�g�Őݒ肷�镶���̈�����wchar_t�̏ꍇ
	// �Q�l http://www.t-net.ne.jp/~cyfis/c/string/TCHAR.html

typedef wchar_t				Nchar;				// �v���W�F�N�g�Őݒ肷�镶���̈����ɍ��킹�ĉ������BUnicode,�}���`�o�C�g
#define Nstrlen(a)			wcslen(a)
#define Nitoa(v,b,l)		_itow(v,b,l)
#define Natof(a)			_wtof(a)

#define Nstrchr(a,b)		wcschr(a,b)
#define Nstrrchr(a,b)		wcsrchr(a,b)		// strrchr()
#define Nstrcpy(a,b)		wcscpy(a,b)			// strcpy()
#define Nstrcpy_s(a,b,c)	wcscpy_s(a,b,c)	
#define	Nstrcat(a,b)		wcscat(a,b)			// strcat()
#define Nstrcat_s(a,b,c)	wcscat_s(a,b,c)
#define	Nstrncmp(a,b,c)		wcsncmp(a,b,c)
#define	Nstrncpy(a,b,c)		wcsncpy(a,b,c)
#define	Nstrncpy_s(a,b,c,d)	wcsncpy_s(a,b,c,d)
#define	N_T(a)				_T(a)				// const char set
#define Nmkdir(a)			_wmkdir(a)


#define Nfopen_s(a,b,c)		_wfopen_s(a,b,c)	// fopen

#endif

#define	Nsprintf(a,b,...)		wsprintf(a,b,__VA_ARGS__)
#define	Nswprintf_s(a,b,c,...)	swprintf_s(a,b,c,__VA_ARGS__)		// int swprintf_s(wchar_t *buffer,size_t sizeOfBuffer,const wchar_t *format,...);


// �^�ϊ�
// std::string std::to_string(int32_t)	// int��std::string�ϊ�

// int32_t _ttoi(CString);				// CString��int�ϊ�

// ::strtol(pszText,&pszEnd,16);		// 16�i������int�ϊ�



#endif	// GNRL_Nchar_H