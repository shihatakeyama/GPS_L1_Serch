// **********************************************************************
// ListCtrlProc.h
// 
// リストボックスのオーナー
// **********************************************************************

#pragma once


class CListCtrlProc 
: public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlProc)

public:
	CListCtrlProc();
	virtual ~CListCtrlProc();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);


	// セル編集
	CEditInListCtrl		m_Edit;
	void BeginEdit(int iItem, int iSubItem);
};


