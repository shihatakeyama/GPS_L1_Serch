// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// CComboInListCtrl.h
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once


// CComboListCtrl

class CComboInListCtrl 
	: public CComboBox
{
	DECLARE_DYNAMIC(CComboInListCtrl)

public:
	CComboInListCtrl();
	virtual ~CComboInListCtrl();

	int m_row;
    int m_col;

	void EndEdit();

protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCbnSelchange();
};


