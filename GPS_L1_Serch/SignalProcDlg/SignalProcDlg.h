
// SignalProcDlg.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CSignalProcDlgApp:
// このクラスの実装については、SignalProcDlg.cpp を参照してください。
//

class CSignalProcDlgApp : public CWinApp
{
public:
	CSignalProcDlgApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()

private:
	ULONG_PTR m_nGdiplusToken;	// ※※※※グラフ

public:
	virtual int ExitInstance();
};

extern CSignalProcDlgApp theApp;