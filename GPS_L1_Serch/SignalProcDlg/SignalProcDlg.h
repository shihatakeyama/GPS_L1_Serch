
// SignalProcDlg.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// CSignalProcDlgApp:
// ���̃N���X�̎����ɂ��ẮASignalProcDlg.cpp ���Q�Ƃ��Ă��������B
//

class CSignalProcDlgApp : public CWinApp
{
public:
	CSignalProcDlgApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()

private:
	ULONG_PTR m_nGdiplusToken;	// ���������O���t

public:
	virtual int ExitInstance();
};

extern CSignalProcDlgApp theApp;