// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ComboInListCtrl.cpp : 実装ファイル
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"
#include "SignalProcDlg.h"
#include "ComboInListCtrl.h"


// CComboListCtrl

IMPLEMENT_DYNAMIC(CComboInListCtrl, CComboBox)

CComboInListCtrl::CComboInListCtrl()
{

}

CComboInListCtrl::~CComboInListCtrl()
{
}


BEGIN_MESSAGE_MAP(CComboInListCtrl, CComboBox)
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CComboInListCtrl::OnCbnSelchange)
END_MESSAGE_MAP()


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// コンボボックス
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CComboInListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	EndEdit();
}

void CComboInListCtrl::OnCbnSelchange()
{
	EndEdit();
}

void CComboInListCtrl::EndEdit()
{
	if(m_hWnd == nullptr)	return;		// 既にウインドウが閉じられた

    if (m_row >= 0 && m_col >= 0){
        CString strText;
        GetWindowText(strText);
        ((CListCtrl*)GetParent())->SetItemText(m_row, m_col, strText);
    }
    DestroyWindow();
}

// CComboInListCtrl メッセージ ハンドラー




