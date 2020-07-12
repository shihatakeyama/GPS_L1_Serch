// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// SignalProcDlgDlg.h : �w�b�_�[ �t�@�C��
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "GnrlInt.h"
#include "SP_Define.h"
#include "afxcmn.h"
#include "ListCtrlProc.h"
#include "afxwin.h"
#include "afxvslistbox.h"


// CGpsL1SearchDlg �_�C�A���O
class CGpsL1SearchDlg : public CDialogEx
{
public:
	enum E_InputSel{
		 EIS_external	= 0
		,EIS_internal	= 1
		,EIS_num		= 2
	};


// �R���X�g���N�V����
public:
	CGpsL1SearchDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SIGNALPROCDLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCudaInfo();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnBnClickedOk();


//--	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	// ������	// ������

	// �Ǝ��̃��b�Z�[�W���M �ǉ�
	afx_msg LRESULT OnSearchCompl(WPARAM wParam, LPARAM lParam);

	// ���X�g�R���g���[��
	CListCtrlProc		m_listctrl;
	CImageList			m_imglstList;

	CStatusBar		    m_StatusBar;

	// �ݒ�t�@�C���ǂݍ���
	int32_t loadDlgSet(const Nchar *FilePath);
	int32_t saveDlgSet(const Nchar *FilePath);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	int m_InputFreq;

//--	afx_msg void OnBnClickedButtonProcListOpen();
//--	afx_msg void OnBnClickedButtonProcListSave();

	CListCtrl m_xcListCtrl;
	int m_StatiBegin;
	afx_msg void OnBnClickedButtonGl();

	afx_msg void OnBnClickedButtonSetIfFilepath();

	int32_t GTS_AccumSum();
	afx_msg void OnBnClickedButtonGlFine();

	// ���ʏ���
	CString m_IfFilePath;
	CString m_DefaultIfFilePath;
//	short m_PrnNo;
	int m_SampleClk;
	BOOL m_CheckPrnAll;
	ULONGLONG m_AccumPeriod;
	ULONGLONG m_SampleOffset;
	int m_SearchMode;

	// �T�[�`����
	int m_CarrFreqCenter;
	int m_CarrStepN;
	int m_CarrStepWidth;
	uint32_t m_CodePhaseReso;	// �R�[�h�ʑ�����\ 1�`

	// ���o����
	int m_DetectLevel;
	int m_DetectCarrFreq;
	int m_DetectCodeChip;
	CString m_DetectCodePhase;
	uint32_t m_DetectCodePhasei;

	// �W�����Č������
	int m_FineCarrStepWidth;
	int m_FineCarrStepN;
	uint32_t m_FineCodePhaseReso;
	int m_FineCodeStepN;
	int m_SvNo;
};
