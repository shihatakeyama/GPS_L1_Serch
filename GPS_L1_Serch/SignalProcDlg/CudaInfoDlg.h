#pragma once


// CCudaInfoDlg ダイアログ

class CCudaInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CCudaInfoDlg)

public:
	CCudaInfoDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CCudaInfoDlg();

// ダイアログ データ
	enum { IDD = IDD_CUDA_INF_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
private:
	CString m_DevicePropertie;
};
