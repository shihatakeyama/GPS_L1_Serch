// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// EditInListCtrl.cpp
// 
// 参考 ： https://www.kazetest.com/vcmemo/listctrl-edit/listctrl-edit.htm
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"
#include "EditInListCtrl.h"


CEditInListCtrl::CEditInListCtrl(int TypeFlg)
:m_TypeFlg(TypeFlg)
{
}


CEditInListCtrl::~CEditInListCtrl()
{
}

void CEditInListCtrl::OnKillFocusAA(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

}
BEGIN_MESSAGE_MAP(CEditInListCtrl, CEdit)
	ON_WM_KILLFOCUS()

	ON_MESSAGE(WM_EDIT_ENTER, OnEditEnter)

	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_CONTROL_REFLECT(EN_CHANGE, &CEditInListCtrl::OnEnChange)
	ON_CONTROL_REFLECT(EN_UPDATE, &CEditInListCtrl::OnEnUpdate)
END_MESSAGE_MAP()


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// セル編集 終了
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CEditInListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

    if (m_row >= 0 && m_col >= 0){
        CString strText;
        GetWindowText(strText);
        ((CListCtrl*)GetParent())->SetItemText(m_row, m_col, strText);
    }
    DestroyWindow();
}
static CString bbbbb;
static int8_t ang=0;
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// キー入力があった
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CEditInListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString cs;
    //入力上限バイト数を取得
    int iLimit = this->GetLimitText();

	// 数値以外入力させない。
	if(m_TypeFlg == 1){
		if(!(((nChar>=0x60) && (nChar<=0x69)) || (nChar==0x08) || (nChar==0x2E))){		// 
			//入力済み文字列を取得
///			this->GetWindowText(bbbbb);
///			ang = 1;
		}
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CEditInListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString cs;
    //入力上限バイト数を取得
    int iLimit = this->GetLimitText();
    //入力済み文字列を取得
    this->GetWindowText(cs);

	if((nChar<'0') || (nChar>'9')){
	//	this->SetWindowText(bbbbb);
	}

	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}
void CEditInListCtrl::OnEnChange()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CEdit::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	// 数値以外入力させない。
	if(m_TypeFlg == 1){
		CString cs;
		this->GetWindowText(cs);
		//数値チェックを行う
		CString csChk = cs.SpanIncluding(_T("0123456789"));

		//半角英数以外が混ざっていたら取り除いて設定する
		if (csChk != cs) {
			TRACE(L"<<Error>> 半角英数以外が混ざっていました [%s]¥r¥n", cs);
			//半角英数以外の文字を取り除く
			int ichi = csChk.GetLength();
			cs.Replace(cs.Mid(ichi,1),L"");
			this->SetWindowText(cs);
		}
	}
}

void CEditInListCtrl::OnEnUpdate()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CEdit::OnInitDialog() 関数をオーバーライドし、
	// EM_SETEVENTMASK メッセージを、
	// OR 状態の ENM_UPDATE フラグを lParam マスクに入れて、このコントロールに送信する必要があります。

	CString tmp;
	this->GetWindowText(tmp);
	if(ang == 1){
//--		this->SetWindowText(bbbbb);
	}
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 他スレッドからデータ受けた場合
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
LRESULT CEditInListCtrl::OnEditEnter(WPARAM wParam, LPARAM lParam)
{
#if 0
	if(m_hWnd == nullptr)	return 0;

    if (m_row >= 0 && m_col >= 0){
        CString strText;
        GetWindowText(strText);
        ((CListCtrl*)GetParent())->SetItemText(m_row, m_col, strText);
    }

    DestroyWindow();

//	ShowWindow(0);
#endif
	OnKillFocus_a(GetParent());

	return 0;
}

BOOL CEditInListCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message){
		case WM_KEYDOWN:
			switch( pMsg->wParam )
			{
				case VK_RETURN:
//					OnKillFocus(this);
					//this->SendMessage(WM_COMMAND ,EN_KILLFOCUS);
///					OnEditEnter(pMsg->message, pMsg->wParam);
//--					SendMessage(WM_EDIT_ENTER,pMsg->message, pMsg->wParam);
					break;
				case VK_ESCAPE:
					return FALSE;
				default:
					break;
			}
			break;
		case WM_COMMAND:
	
		if (pMsg->wParam == EN_SETFOCUS){
			int aa = 1;
		}else if (pMsg->wParam == EN_KILLFOCUS){
			int aa = 1;
		}
		break;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

