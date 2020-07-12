// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// OS�Ȃǂ��񋟂��鐔�X�̃��W���[�������b�v����B
// GnrlFilePath.cpp
// 
// 2019/10/14  Createed
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#include "stdafx.h"

#include <stdio.h>
#include <direct.h>

#include "GnrlDefine.h"
#include "GnrlFilePath.h"


///#ifndef MAX_BUF		// �o�b�t�@�T�C�Y�@�L���ȃT�C�Y�ׁ̈A�p�ɂɎg�p����ׂ��łȂ��B
//#define MAX_BUF	256
//#endif
//typedef wchar_t	Nchar;	// �v���W�F�N�g�Őݒ肷�镶���̈����ɍ��킹�ĉ������BUnicode,�}���`�o�C�g

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  ���s�t�@�C�����u����Ă���t�@�C���p�X��Ԃ��܂�
//  �֐�����������ƁA�o�b�t�@�ɃR�s�[���ꂽ������̒������A�����P�ʂŕԂ�
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t GnrlFilepath::getModuleFilePath( Nchar *FilePath, size_t Size)
{
	return ::GetModuleFileName(NULL ,FilePath ,Size);

#if 0	// Linux�̏ꍇ�̎Q�l
    char buf[1024];
    int ret = readlink("/proc/self/exe", buf, sizeof(buf));
    if (ret != -1) {
        buf[ret] = '\0';
        printf("path=%s\n", buf);
    }
>path=/home/shigeo/a.out
#endif
}

// **********************************************************************************
//  ���s�t�@�C�������u����Ă���p�X����ɂ����A���΃p�X���΃p�X�ɕϊ�����B
//  ����
//�@  FilePath out	��΃p�X
//�@  FileName in		���΃t�@�C�������w��,��΃p�X���w��\
//  �߂�l
//�@  ����������΃p�X�iFilePath�j�̒�����Ԃ��܂��B ==0:�G���[
// **********************************************************************************
uint32_t GnrlFilepath::getModuleAttachmentFilePath(Nchar* FilePath, size_t Size, const Nchar *FileName)
{
	size_t size;
	Nchar	*cbp;

	// �����s�t�@�C���̃p�X���擾
	size = getModuleFilePath(FilePath, Size);

	if( size == 0)
		return size;

	// ���s�t�@�C�������݂���f�B���N�g�����擾
	getBaseDirectory(cbp, FilePath);

	if(cbp == NULL)
		return 0;

	*cbp = '\0';

	toAbsolutePath(FilePath, Size ,FilePath, FileName);

	return Nstrlen(FilePath);
}

// **********************************************************************************
//  BaseFirst�����݂���f�B���N�g����&Result�ɕԂ��B
//  �{�֐��ł́Aconst�͂��Ă��܂��񂪁AFirst�ALase�̕�����̑�����s���܂���B
// ����
//   First	  in	������̐擪�|�C���^
//   Last	  in	������̍Ō�̃|�C���^ First�ɐݒ肳�ꂽ������NULL�I�[�ł���ꍇ�́ANULL��ݒ�\�B
//   TerminalYRemoved TRUE:������I�[��'\'������ꍇ�A��菜���ď�������
// Result	  out	\�܂���/�̈ʒu������
// **********************************************************************************
void GnrlFilepath::getBaseDirectory(Nchar* &Result, Nchar *First, Nchar *Last, bool TerminalYRemoved)
{
	Nchar *p;

	if(First==NULL || First[0]=='\0'){
		Result = NULL;
		return;
	}

	// Base�̏I�[�A�h���X���i�[
	if(Last!=NULL)
	{
		p = Last;
	}else{
		p = First + Nstrlen(First);
	}

	if(TerminalYRemoved)
		if(*p=='\\' || *p=='/')		// �����\�A/�͎�菜��
			p--;

	while(First < p){
		if(*p==':'){		// C:
			Result = Last;
			return;
		}
		
		if(*p=='\\'||*p=='/'){	
			Result = p;//        ��Last��\���w��
			return;		// C:\hoge\�@
		}
		p--;
	}

	Result = Last;
	return;
}


#if 0
// (Path)���΃p�X�ɕϊ�����
void GnrlFilepath::toAbsolutePath(int8_t *Result, 
							const int8_t *Base,
							const int8_t *Path)
{
	int32_t length;
	const int8_t *pp,*bp;	// �p�X�|�C���^�A�x�[�X�|�C���^
	int8_t *rp;

	// path����΃p�X�\�L�ł���ꍇ
	if(Path[1]==':')
	{
		strcpy(Result, Path);	// Path�̒l��Ԃ�
		return;
	}

#if 0
	// path���J�����g�f�B���N�g���ł���ꍇ
	if(!strcmp(Path,".")||!strcmp(path,".\"))
	{
		strcpy(Result,Path);
		return;
	}
#endif

	length = strlen(Base);
	bp = Base + length;		// bp�͕�����base�̏I�[���w��
	if(*(bp-1)=='\\' || *(bp-1)=='/')
		bp--;				// Base�̌����\�A/�͎�菜��

	// Path�̐擪���ɂ���".."�܂���"../"����͂��鎖��
	// Base�̃p�X�\�L�̂����AResult�Ƌ��ʕ����𒲂ׂ�
	for(pp=Path; *pp!='\0' && *pp=='.';)
	{
		if(!strncmp(pp,"..\\",3)||!strncmp(pp,"../",3))	// Path�O����[..\]1�K�w��ֈړ��\
		{
			pp += 3;
			getBaseDirectory(bp, Base, bp);
		}else if(!strncmp(pp,".\\",2) || !strncmp(pp,"./",2))
		{
			pp +=2;
		}else if(!strncmp(pp,"..\0",3))
		{
			pp += 2;
			getBaseDirectory(bp, Base,bp);
		}else
		{
			break;
		}
	}

	length = bp - Base;				// Base�̃p�X���Ƌ��ʕ�����Result�ɕ��ʂ���
	strncpy(Result,Base,length);

	// Base��Path�̊Ԃ̕�����}��
	rp = Result + length;
	if(strchr(Base,'\\') || strchr(Path,'\\'))
	{
		*rp++ = '\\';
	}else
	{
		*rp++ = '/';
	}

	// Path�̂����A�擪������".\"��"..\"����菜�����c��̕���
	// (pp������������)���AResult�̕�����ɒǉ�����B
	strcpy(rp,pp);
	return;
}
#else
void GnrlFilepath::toAbsolutePath(Nchar *Result, int32_t Len,
							const Nchar *Current,
							const Nchar *Path)
{
	Nchar *ct = (Nchar*)Current;
	int32_t length;
	Nchar	*pp,*bp;	// �p�X�|�C���^�A�x�[�X�|�C���^
	Nchar	*rp;
	Nchar	subdircode;		// '\'�܂���'/'

	if(Nstrchr(Current,'\\')){
		subdircode = '\\';
	}else{
		subdircode = '/';
	}

#if 0
	if(!strcmp(Path,".")||!strcmp(path,".\\"))	// path���J�����g�f�B���N�g���ł���ꍇ
	{
		strcpy(Result,Path);
		return;
	}
#endif
	if(*Path == subdircode){	// Path�����[�g�f�B���N�g���w��̏ꍇ

		Nstrcpy_s(Result, Len, Current);
		rp = Nstrchr(Result, subdircode);
		if(rp && rp[1]==subdircode){
			// Current���@http:// �ł���ꍇ
			rp = Nstrchr(rp+3, subdircode);
			if(rp){
				Nstrcpy_s(rp, Len - (rp - Result), Path);	// "http://yasuneko/"�̌���Path"Name"��t��
				return;
			}else{
				Nstrcat_s(Result, Len , Path);				// "http://yasuneko"�̌���Path"/Name"��t��
				return;
			}
		}else{
			// Current���@C:\�@//
			Nstrcpy_s(rp, Len - (rp - Result), Path);
			return;
		}
	}

	// Path����΃p�X�\�L�ł���(�t�@�C���p�X�̍���������)�ꍇ
	// �l�b�g���[�N�A�N�Z�X�̏ꍇ�͍l��
	if(Nstrchr(Path, ':') != NULL
	||( Path[0]==subdircode && Path[1]==subdircode)){
		Nstrcpy_s(Result, Len, Path);	// Path�̒l��Ԃ�
		return;
	}

	length = Nstrlen(Current);
	bp = ((Nchar*)Current) + length;	// bp�͕�����base�̏I�[���w��
	if(*(bp-1)=='\\' || *(bp-1)=='/'){
		bp--;				// Base�̌����\�A/�͎�菜��
	}

	// Path�̐擪���ɂ���".."�܂���"../"����͂��鎖��
	// Base�̃p�X�\�L�̂����AResult�Ƌ��ʕ����𒲂ׂ�
	for(pp=(Nchar*)Path; *pp!='\0' && *pp=='.';){
		if(!Nstrncmp(pp ,N_T("..\\") ,3) || !Nstrncmp(pp ,N_T("../") ,3)){	// Path�O����[..\]1�K�w��ֈړ��\
			pp += 3;
			getBaseDirectory(bp, ct, bp);
		}else if(!Nstrncmp(pp ,N_T(".\\"),2) || !Nstrncmp(pp ,N_T("./") ,2)){
			pp +=2;
		}else if(!Nstrncmp(pp ,N_T("..\0") ,3)){
			pp += 2;
			getBaseDirectory(bp, ct, bp);
		}else{
			break;
		}
	}

	// Base�̃p�X���Ƌ��ʕ�����Result�ɕ��ʂ���
	length = bp - Current;
//--	Nstrncpy(Result, Current, length);			// 
	wcsncpy_s(Result, Len, Current, length);		//

	// Base��Path�̊Ԃ̕�����}��
	rp = Result + length;
//	if(strchr(Current,'\\') || strchr(Path,'\\'))
//	{
//		*rp++ = subdircode;//'\\';
//	}else
//	{
//		*rp++ = subdircode;// '/';
//	}
	*rp++ = subdircode;

	// Path�̂����A�擪������".\"��"..\"����菜�����c��̕���
	// (pp������������)���AResult�̕�����ɒǉ�����B
	Nstrcpy_s(rp, Len - (rp - Result), pp);

	return;
}
#endif

#if 0 // �R���p�C���G���[�o��̂ŁB�B
// **********************************************************************************
//  ��΃p�X�𑊑΃p�X�ɕϊ�����
//
//  ����
//    RelativePath ���΃p�X�o��
//    AbsolutePath ��΃p�X����
//    CurrentBase  �J�����g�p�X����
//    DriveEnable�@FALSE:�h���C�u�̕ϊ������@TRUE:�h���C�u�̕ϊ��L���iAbsolutePath�Ńh���C�u�̎w�肪�L���ɂ���ꍇ�j
// **********************************************************************************
void GnrlFilepath::toRelativePath(Nchar *RelativePath, const Nchar *AbsolutePath, const Nchar *CurrentBase, Bool DriveEnable)
{
	const Nchar *apath,*cbase;
	Nchar *upsequence,sepsequence;
	const Nchar *acoron,*ccoron;
	int32_t	len,i;
	
	// �������Path�����΃p�X�ł���ꍇ
	acoron = Nstrrchr(AbsolutePath,':');
	ccoron = Nstrrchr(CurrentBase,':');
	if(ccoron == NULL || acoron == NULL){
		Nstrcpy(RelativePath, AbsolutePath);	// ���̂܂܏o��
		return;
	}

	if(Nstrrchr(AbsolutePath, '\\') == NULL){
		upsequence = N_T("../");
		sepsequence = '/';
	}else{
		upsequence = N_T("..\\");
		sepsequence = '\\';
	}

	if(DriveEnable){	// �h���C�u�̕ύX��L���ɂ���ꍇ
		// ***�h���C�u�ϊ����L���ł���ꍇ���΃p�X�̃h���C�u���g�p����***
		len = acoron - AbsolutePath;
		if(len == (ccoron-CurrentBase)){
			i = 0;
			while(1){
				if(i >= len ){
					break;
				}
				if(tolower(*AbsolutePath) != tolower(*CurrentBase)){
					// �h���C�u���s��v�̏ꍇ��΃p�X���R�s�[���ă��^�[������
					Nstrcpy(RelativePath, AbsolutePath);
					return;
				}
				i++;
			}
		}else{
			Nstrcpy(RelativePath, AbsolutePath);
			return;
		}
		// �h���C�u�ϊ��������ł���ꍇ��΃p�X�̃h���C�u���g�p����	
//		memcpy(RelativePath, CurrentBase, (ccoron+1 - CurrentBase));
	}
	
	// �h���C�u�͍l�����Ȃ��Ă����̂ŁA�ǂ̂��炢�̃l�X�g�܂ň�v���邩�𒲂ׂ�
	RelativePath[0] = '\0';
	apath = acoron + 1;		// ��΃p�X
	cbase = ccoron + 1;		// ���΃p�X
	
	while(*apath){
		if(*apath=='\\' || *apath=='/'){
			apath++;
		}
	}
	while(*cbase){
		if(*cbase=='\\' || *cbase=='/'){
			cbase++;
		}
	}

	while(1)
	{
		if(*acoron==sepsequence){	// �ǂ̃f�B���N�g���l�X�g�܂œ����Ȃ̂���������
			acoron = apath+1;
		}

		if(cbase=='\0')
		{
			Nstrcat(RelativePath, upsequence);
			Nstrcat(RelativePath, acoron);
			return;
		}

		if(tolower(*AbsolutePath) != tolower(*CurrentBase))
		{	// �����񂪈���Ă�����A�c��̃l�X�g���𒲂ׂăV�[�P���X��ǉ�����
			Nstrcat(RelativePath, upsequence);
			
			while(1){
				if(*cbase==NULL)	break;

				if(*cbase==sepsequence && cbase[1]!= '\0'){
					Nstrcat(RelativePath, upsequence);
				}
				cbase++;
			}
			Nstrcat( RelativePath, upsequence+2);	// "\"�܂���"/"��ǉ�
			Nstrcat(RelativePath, acoron);
			return;
		}
		apath++;
		cbase++;	
	}
}
#endif

// �f�B���N�g���̑��݂��m�F����
bool GnrlFilepath::isDirectory(const Nchar* DirName)
{
	uint32_t result;

	result = GetFileAttributes(DirName);

	if(result==0xFFFFFFFF)
	{
		return FALSE;
	}else if(result & FILE_ATTRIBUTE_DIRECTORY)
	{
		return TRUE;
	}else{
		return FALSE;
	}
}
// �p�X�̎�ʂ��擾����
uint32_t GnrlFilepath::getFileAttributes(const Nchar* DirName)
{
	return GetFileAttributes(DirName);
}

#if 0	// �R���p�C���G���[�o��̂ŁB�B
// �f�B���N�g�����쐬���܂��B������Ƀf�B���N�g�������݂���ꍇTRUE��߂��܂�
Bool GnrlFilepath::makeDir(const Nchar* DirName)
{
	Nchar buf[MAX_PATH],*buflast;
	Nchar *bufsep;
	int32_t loopnew,loopold=0x7FFFFFFF;

	// �����Ƀf�B���N�g�������݂��Ă��Ȃ����`�F�b�N
	if(isDirectory(DirName)==TRUE)
		return TRUE;

	do{
		Nstrncpy(buf, DirName, MAX_PATH);
		buflast = buf + Nstrlen(buf);

		for(;DirName==0;DirName--)	// DirName���牽�K�w�T�u�f�B���N�g�������Ȃ��H
		{
			getBaseDirectory(bufsep, buf, buflast);
			buflast = (Nchar*)bufsep;
			*((Nchar*)bufsep) = '\0';
		}

		loopnew = 0;
		while(Nmkdir(buf)!=0)
		{	// �f�B���N�g���̍쐬�Ɏ��s������A�f�B���N�g����Up������
			int32_t err = errno;

			getBaseDirectory(bufsep, buf, buflast);
			if(bufsep == buflast)
				return FALSE;	// �f�B���N�g�����쐬�ł��Ȃ� DirName�̒l���ُ�
			*((int8_t*)bufsep) = '\0';
			buflast = (Nchar*)bufsep;
			loopnew++;
		}
		if(loopnew<loopold)	// _mkdir()�A���G���[�ɂ�閳�����[�v���΍�
		{
			loopold = loopnew;
		}else{
			return FALSE;
		}
	}while(loopnew!=0);

	return TRUE;
}
#endif

//**********************************************************************************
// �t�@�C�����R�s�[����
//**********************************************************************************
bool GnrlFilepath::copyFile(const Nchar* ExistingFilePath, const Nchar* NewFilePath, bool FailIfExists)
{
	if(::CopyFile(ExistingFilePath, NewFilePath, FailIfExists))
	{return true;}
	else{return false;}
}

#if 1
//**********************************************************************************
// �t�@�C�����폜����
//**********************************************************************************
bool GnrlFilepath::deleteFile(const Nchar* FilePath)
{
	if(DeleteFile(FilePath)){
		return TRUE;
	}else{
		return FALSE;	
	}
}
#endif


// �p�X����v���Ă��邩�`�F�b�N����	Ref�ɂ̓��C���J�[�h���܂߂鎖���\ �啶���������̋�ʂ͍s��Ȃ�
// ����
//		Path	�t���p�X����				��A"c:\Program\bin\hoge.exe"
//		Ref		�����p�p�X(���C���J�[�h�\)��A"c:\Program\*"
// �߂�l
//		��v���Ă���ꍇ�ATRUE��Ԃ��܂�
bool GnrlFilepath::compPath(const Nchar *Path, const Nchar *Ref)
{
	const Nchar *path;

	if(!Path || !Ref)	return false;

	while(1){
		if(*Ref == '\0'){
			if(*Path == '\0'){
				return true;
			}else{
				return false;
			}
		}else if(*Ref == '*'){		// '*'���C���J�[�h����
			while(*Ref == '*')
				Ref++;

			if(*Ref == '\0')
			{
				return true;	// '*'�̌��'\0'���������ꍇ�A��v����Ƃ���
			}else{
				path = Path;
//				Ref++;		// '*'���C���J�[�h�̕����񒷂�����
				while(*path)
				{
					if( //tolower(*path) == tolower(*Ref)	/* �����������ɂ���ׂɒǉ� */
						//&&
						compPath(path, Ref)
						)
						return true;
					path++;
				}
			}
		}else{
			if(*Path == '\0')
			{
				return false;
			}
		}

		if(tolower(*Path) != tolower(*Ref)							// ������v
		&& !((*Path == '\\' || *Path == '/') && (*Ref == '\\' || *Ref == '/'))	// �f�B���N�g���V�[�P���X
		&& *Ref != '?'){	// '?'���C���J�[�h
			return false;
		}else{
			Path++;
			Ref++;
		}
	}
}

bool GnrlFilepath::moveFile(const Nchar* ExistingFilePath, const Nchar* NewFilePath)
{
	if(::MoveFile(ExistingFilePath, NewFilePath)){
		return true;
	}else
	{
		return false;
	}
}
