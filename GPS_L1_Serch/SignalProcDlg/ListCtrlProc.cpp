// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ListCtrlOwner.cpp : �����t�@�C��
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"
#include "SignalProcDlg.h"
#include "ListCtrlProc.h"


// CListCtrlProc

IMPLEMENT_DYNAMIC(CListCtrlProc, CListCtrl)

CListCtrlProc::CListCtrlProc()
{
}

CListCtrlProc::~CListCtrlProc()
{
}


BEGIN_MESSAGE_MAP(CListCtrlProc, CListCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CListCtrlProc::OnNMDblclk)
END_MESSAGE_MAP()



// CListCtrlProc ���b�Z�[�W �n���h���[


void CListCtrlProc::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;

	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	GetItem(&lvi);
	//���s�̔w�i
	COLORREF clrTextSave;
	COLORREF clrBkSave;
	if (lvi.state & LVIS_SELECTED){
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		CBrush cb(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcItem, &cb);
	}else{
		if (nItem % 2)    //�Ȕn�͗l
		{
			CBrush cb(::GetSysColor(COLOR_WINDOW));
			pDC->FillRect(rcItem, &cb);
		}
		else
		{
			CBrush cb((COLORREF)0x00f0f0f0);
			pDC->FillRect(rcItem, &cb);
		}
	}
	//�r�b�g�}�b�v�̕`��
	CImageList* pImageList = GetImageList(LVSIL_SMALL);
	pImageList->Draw(pDC, lvi.iImage, rcItem.TopLeft(), ILD_TRANSPARENT);

	CRect rectItem(0, 0, 0, 0);
	CRect rectText;

	for (int j = 0; j < 3; j++){
		if (j == 0)
			GetItemRect(nItem, rectItem, LVIR_LABEL);
		else
		{
			rectItem.left = rectItem.right;
			rectItem.right += GetColumnWidth(j);
		}

		rectText = rectItem;
		rectText.right -= 4;

		CString strData = GetItemText(nItem, j);

		if ((lvi.state & LVIS_SELECTED))
			clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));

		if (rectText.right >= rectText.left)
			pDC->DrawText(strData, rectText, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

		if ((lvi.state & LVIS_SELECTED))
			pDC->SetTextColor(clrTextSave);
	}

	if (lvi.state & LVIS_SELECTED){
		pDC->SetBkColor(clrBkSave);
	}
	//�t�H�[�J�X�̕`��
	if ((lvi.state & LVIS_FOCUSED) != 0 && GetFocus() == this){
		pDC->DrawFocusRect(&rcItem);
	}

}


BOOL CListCtrlProc::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	int DisplayMode = 0;// atoi(GetRegistry(DisplayMode));

	// 0:�A�C�R�� 1:�ꗗ 2:�ڍ�
	if (DisplayMode == 0)
		cs.style |= (LVS_ICON | LVS_SHOWSELALWAYS | LVS_OWNERDATA);//�A�C�R��

	if (DisplayMode == 1)
		cs.style |= (LVS_LIST | LVS_SHOWSELALWAYS | LVS_OWNERDATA);//�ꗗ

	if (DisplayMode == 2)
		cs.style |= (LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA);//�ڍ�

	cs.dwExStyle |= (LVS_EX_TWOCLICKACTIVATE |
		LVS_EX_INFOTIP |
		LVSICF_NOINVALIDATEALL |
		LVSICF_NOSCROLL);

	return CListCtrl::PreCreateWindow(cs);
}


BOOL CListCtrlProc::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	return CListCtrl::PreTranslateMessage(pMsg);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �������X�g���_�u���N���b�N���ꂽ�B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CListCtrlProc::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	 LV_HITTESTINFO lvinfo;
//	LPNMLISTVIEW	lp;

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);


    GetCursorPos((LPPOINT)&lvinfo.pt);
    ScreenToClient(&lvinfo.pt);	// ((LPNMLISTVIEW)pNMItemActivate->lParam)->hdr.hwndFrom, 
	SubItemHitTest(&lvinfo);
#if 0
    if ((lvinfo.flags & LVHT_ONITEM) != 0)
    {
        item.mask = TVIF_HANDLE | TVIF_TEXT;
        item.iItem = lvinfo.iItem;
        item.iSubItem = 0;          //�擾����T�u�A�C�e���̔ԍ�
        item.pszText = buf;         //�i�[����e�L�X�g�o�b�t�@
        item.cchTextMax = 256; //�o�b�t�@�e��
		lp = ((LPNMLISTVIEW)pNMItemActivate->lParam);	// ????
        ListView_GetItem(pNMItemActivate->hdr.hwndFrom, &item);
        MessageBox( buf, TEXT("�I�������f�[�^"), MB_OK);
    }
#endif


#if 0	// �{�ԗp

	// �p�����[�^
	if(lvinfo.iItem < (int)gProcessList.size()){
		gProcessList[lvinfo.iItem]->showSetDlg();
	}	

#endif

	*pResult = 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �Z���ҏW �J�n
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define IDC_EDIT_SUB_ITEM 1234
void CListCtrlProc::BeginEdit(int iItem, int iSubItem)
{
    CRect ColumnRect;
    
    if (iSubItem == 0)
        GetSubItemRect(iItem, iSubItem, LVIR_LABEL, ColumnRect);
    else
        GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, ColumnRect);

    ColumnRect.DeflateRect(1,1);

    m_Edit.Create(WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT, ColumnRect,  this, IDC_EDIT_SUB_ITEM);
    CFont* pFont = GetFont();
    m_Edit.SetFont(pFont);
    m_Edit.m_row = iItem;
    m_Edit.m_col = iSubItem;
    m_Edit.SetWindowText(GetItemText(iItem, iSubItem));
    m_Edit.SetFocus();
}
