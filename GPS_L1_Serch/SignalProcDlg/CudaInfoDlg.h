#pragma once


// CCudaInfoDlg �_�C�A���O

class CCudaInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CCudaInfoDlg)

public:
	CCudaInfoDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CCudaInfoDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_CUDA_INF_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
private:
	CString m_DevicePropertie;
};
