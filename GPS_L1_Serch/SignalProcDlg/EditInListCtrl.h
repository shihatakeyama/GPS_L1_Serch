// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// EditInListCtrl.h
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#pragma once
#include "afxwin.h"
class CEditInListCtrl :
	public CEdit
{
public:
	CEditInListCtrl(int TypeFlg=0);
	~CEditInListCtrl();

	afx_msg void OnKillFocusAA(CWnd* pNewWnd);
	void EndEdit();

	int m_TypeFlg;
    int m_row;
    int m_col;
	DECLARE_MESSAGE_MAP()

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	// 独自のメッセージ送信 追加
	afx_msg LRESULT OnEditEnter(WPARAM wParam, LPARAM lParam);

public:
	void OnKillFocus_a(CWnd* pNewWnd)	{ return OnKillFocus(pNewWnd);	}

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnChange();
	afx_msg void OnEnUpdate();
};

