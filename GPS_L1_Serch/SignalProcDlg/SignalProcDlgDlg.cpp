// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// SignalProcDlgDlg.cpp : 実装ファイル
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <stdint.h>
#include "afxdialogex.h"
#include "SignalProcDlg.h"
#include "CudaInfoDlg.h"
#include "ComboInListCtrl.h"

#include "GnrlDefine.h"
#include "GnrlCharset.h"
#include "GnrlFilePath.h"

#include "SignalProcDlgDlg.h"

#include "SP_Define.h"

#include "CudaMem.h"

#include "L1CaCode.h"


using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGpsL1SearchDlg *gMainWind = nullptr;


static BOOL WritePrivateProfileInt(const LPCTSTR lpAppName, const LPCTSTR lpKeyName, int Val, const LPCTSTR lpFileName);

//--int32_t GTS_CorrTest(LineDataSet &Lds);
//--int32_t GTS_FFT_Test(LineDataSet &Lds);
void L1makeCaCodelist();

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// iniファイル読み込み 文字列
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
DWORD GetPrivateProfileStringA(
    _In_opt_ LPCWSTR lpAppName,
    _In_opt_ LPCWSTR lpKeyName,
    _In_opt_ LPCWSTR lpDefault,
	CString &ReturnString,//    _Out_writes_to_opt_(nSize, return + 1) LPWSTR lpReturnedString,
    _In_opt_ LPCWSTR lpFileName
    )
{
	DWORD ack;
	WCHAR buf[256];

	ack = GetPrivateProfileString(lpAppName ,lpKeyName ,lpDefault ,buf ,sizeof(buf) ,lpFileName);

	ReturnString = buf;

	return ack;
}
static BOOL WritePrivateProfileInt(const LPCTSTR lpAppName, const LPCTSTR lpKeyName, int Val, const LPCTSTR lpFileName)
{
	Nchar	str[32];

	Nswprintf_s(str,32 , N_T("%d"), Val);

	return ::WritePrivateProfileString(lpAppName, lpKeyName, str, lpFileName);
}

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
	CString m_AppliVersion;
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
, m_AppliVersion(_T(""))
{
	m_AppliVersion.Format(_T("%2d.%02d") ,APPLI_VERSION/100 ,APPLI_VERSION%100);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_AppliVersion);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CGpsL1SearchDlg::CGpsL1SearchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGpsL1SearchDlg::IDD, pParent)
	, m_hIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME))
	, m_InputFreq(1000)
	, m_CodePhaseReso(2)
	, m_StatiBegin(-100)
	, m_IfFilePath(_T(""))
	, m_SvNo(0)
	, m_SampleClk(16368000)
	, m_CheckPrnAll(FALSE)
	, m_AccumPeriod(0)
	, m_SampleOffset(0)
	, m_CarrFreqCenter(4092000)
	, m_CarrStepN(0)
	, m_CarrStepWidth(0)
	, m_DetectLevel(0)
	, m_DetectCarrFreq(0)
	, m_DetectCodeChip(0)
	, m_DetectCodePhase(_T("0"))
	, m_DetectCodePhasei(0)
	, m_FineCarrStepWidth(0)
	, m_FineCarrStepN(0)
	, m_FineCodeStepN(0)
	, m_FineCodePhaseReso(2)
{
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGpsL1SearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST2, m_listctrl);
	//  DDX_Text(pDX, IDC_EDIT_CODE_STEP_N, m_CodeStepWidth);
	DDX_Text(pDX, IDC_EDIT_CODE_STEP_N, m_CodePhaseReso);	// サーチ Code 分解能
	DDV_MinMaxInt(pDX, m_CodePhaseReso, 1, 1024);

	DDV_MinMaxInt(pDX, m_StatiBegin, -1000000, 1000000);

	DDX_Text(pDX, IDC_EDIT_IF_FILEPATH, m_IfFilePath);
	DDX_Text(pDX, IDC_EDIT_SAMPLE_CLK, m_SampleClk);
	DDX_Check(pDX, IDC_CHECK_PRN_ALL, m_CheckPrnAll);
	DDX_Text(pDX, IDC_EDIT_ACCUM_PERIOD, m_AccumPeriod);
	DDX_Text(pDX, IDC_EDIT_SAMPLE_OFFSET, m_SampleOffset);
	DDX_Text(pDX, IDC_EDIT_CARR_CENTER, m_CarrFreqCenter);
	DDX_Text(pDX, IDC_EDIT_CARR_STEP_N, m_CarrStepN);
	DDX_Text(pDX, IDC_EDIT_CARR_STEP_WIDTH, m_CarrStepWidth);
	DDX_Text(pDX, IDC_EDIT_DETECT_LEVEL, m_DetectLevel);
	DDX_Text(pDX, IDC_EDIT_DETECT_CARR_FREQ, m_DetectCarrFreq);
	DDX_Text(pDX, IDC_EDIT_DETECT_CODE_CHIP, m_DetectCodeChip);
	DDX_Text(pDX, IDC_EDIT_FINE_CARR_STEP_WIDTH, m_FineCarrStepWidth);
	DDX_Text(pDX, IDC_EDIT_FINE_CARR_STEP_N, m_FineCarrStepN);
	DDX_Text(pDX, IDC_EDIT_FINE_CODE_CENTER_PHASE, m_FineCodePhaseReso);
	DDV_MinMaxInt(pDX, m_FineCodePhaseReso, 1, 1024);
	DDX_Text(pDX, IDC_EDIT_FINE_CODE_STEP_N, m_FineCodeStepN);
	DDV_MinMaxInt(pDX, m_FineCodeStepN, 0, 1024);
	DDX_Text(pDX, IDC_EDIT_DETECT_CODE_PHASE, m_DetectCodePhase);
	DDX_CBIndex(pDX, IDC_COMBO_SVNO, m_SvNo);
}

BEGIN_MESSAGE_MAP(CGpsL1SearchDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//--	ON_BN_CLICKED(IDC_BUTTON_GEN, &CGpsL1SearchDlg::OnBnClickedButtonGen)
	ON_BN_CLICKED(IDC_BUTTON_CUDA_INFO, &CGpsL1SearchDlg::OnBnClickedButtonCudaInfo)
	ON_WM_DROPFILES()
//--	ON_BN_CLICKED(IDC_BUTTON2, &CGpsL1SearchDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CGpsL1SearchDlg::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()

	// 独自のメッセージ送信 追加
	ON_MESSAGE(WM_COM_RECV_DATA, OnSearchCompl)

	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_BN_CLICKED(IDC_BUTTON_GL, &CGpsL1SearchDlg::OnBnClickedButtonGl)

	ON_BN_CLICKED(IDC_BUTTON_SET_IF_FILEPATH, &CGpsL1SearchDlg::OnBnClickedButtonSetIfFilepath)
	ON_BN_CLICKED(IDC_BUTTON_GL_FINE, &CGpsL1SearchDlg::OnBnClickedButtonGlFine)
//	ON_BN_CLICKED(IDC_BUTTON_GEN, &CGpsL1SearchDlg::OnBnClickedButtonGen)
END_MESSAGE_MAP()


// CGpsL1SearchDlg メッセージ ハンドラー

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ステータスバー
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_ORG,	//Stringリソースで作成したID
};
void setStatusBar(CGpsL1SearchDlg &Window)
{
	int nHeight = 19;
	CRect rectClient;
	// ステータスバー

	int nStatusHeight = 0;
	if (Window.m_StatusBar.GetSafeHwnd()){// コントロールの有効性をチェックします。

		CRect rectSt;
		Window.m_StatusBar.GetWindowRect(rectSt);
		// 現在のクライアントウィンドウのサイズを取得
		Window.GetClientRect(&rectClient);
		// 高さを維持しながらサイズを変更する
		int nBtm = nHeight;	//	rectSt.bottom - rectSt.top;
		rectSt.top = rectClient.bottom - nBtm;
		rectSt.bottom = rectSt.top + nBtm;
		rectSt.left = rectClient.left;
		rectSt.right = rectClient.right;
		Window.m_StatusBar.MoveWindow(rectSt);
		nStatusHeight = rectSt.Height();
	}
}

BOOL CGpsL1SearchDlg::OnInitDialog()
{
	uint32_t ack;
	int32_t iack,i;

	CDialogEx::OnInitDialog();

	gMainWind = this;

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// **** ステータスバー ****
	// ステータスバーを作成・付加
	m_StatusBar.Create(this);
	m_StatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	setStatusBar(*this);


	L1makeCaCodelist();

    HWND hCombo = ::GetDlgItem(m_hWnd ,IDC_COMBO_SVNO);

    // コンボボックスにデータを詰めていく
	i=0;
    while(1){
		const struct L1PrnTap *itr = L1StaticPrn::getSv(i);
		if(itr == nullptr)	break;
		std::wstring str = std::to_wstring(itr->Prn);
        ::SendMessage(hCombo , CB_ADDSTRING , 0 , (LPARAM)str.c_str());
		i++;
    }


	Nchar buftext[MAX_PATH];

	ack = GnrlFilepath::getModuleAttachmentFilePath(buftext, MAX_PATH ,N_T("SignalProc.ini"));
	if(ack <= 0){
		return -3;
	}
	m_DefaultIfFilePath = buftext;

	loadDlgSet(m_DefaultIfFilePath);

	iack = cudaLoad(gIfData , static_cast<std::wstring>(m_IfFilePath));	// CString → wstring変換
	if(iack < 0){
		m_IfFilePath.Empty();
		UpdateData(FALSE);
	}

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}


void CGpsL1SearchDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	saveDlgSet(m_DefaultIfFilePath);

}

void CGpsL1SearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}else{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。
void CGpsL1SearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CGpsL1SearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// **********************************************************************
//  OK押された
// **********************************************************************
void CGpsL1SearchDlg::OnBnClickedOk()
{

	if(::GetFocus() == ::GetDlgItem(m_hWnd ,IDOK)){
		CDialogEx::OnOK();	// Enterで閉じさせない。
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 設定ファイル読み込み表示/表示内容書き込み
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int32_t CGpsL1SearchDlg::saveDlgSet(const Nchar *FilePath)
{
	Nchar	sectionbuf[256];

	UpdateData(TRUE);

	wcscpy(sectionbuf ,N_T("COMMON"));
	WritePrivateProfileString(sectionbuf,N_T("IfFilepath")	,m_IfFilePath	,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("SvNo")		,m_SvNo			,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("CheckAllPrn")	,m_CheckPrnAll	,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("SampleClk")	,m_SampleClk	,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("SampleOffset"),m_SampleOffset	,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("AccumPeriod")	,m_AccumPeriod	,FilePath);

	wcscpy(sectionbuf ,N_T("SEARCH"));
	WritePrivateProfileInt(sectionbuf	,N_T("CarrerCenter")	,m_CarrFreqCenter	,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("CarrerStepWidth")	,m_CarrStepWidth	,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("CarrerStepN")		,m_CarrStepN		,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("CodePhaseReso")	,m_CodePhaseReso	,FilePath);

	wcscpy(sectionbuf ,N_T("FINE"));
	WritePrivateProfileInt(sectionbuf	,N_T("CarrerStepWidth")	,m_FineCarrStepWidth,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("CarrerStepNum")	,m_FineCarrStepN	,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("FineCodeReso")	,m_FineCodePhaseReso,FilePath);
	WritePrivateProfileInt(sectionbuf	,N_T("CodeStepNum")		,m_FineCodeStepN	,FilePath);

	return 0;
}
int32_t CGpsL1SearchDlg::loadDlgSet(const Nchar *FilePath)
{
	Nchar	sectionbuf[256];

	Nstrcpy(sectionbuf ,N_T("COMMON") );
	GetPrivateProfileStringA(sectionbuf ,N_T("IfFilepath") ,N_T("")			,m_IfFilePath ,FilePath);
	m_SvNo			= GetPrivateProfileInt(sectionbuf ,N_T("SvNo")			,1			,FilePath);
	m_CheckPrnAll	= GetPrivateProfileInt(sectionbuf ,N_T("CheckAllPrn")	,0			,FilePath);
	m_SampleClk		= GetPrivateProfileInt(sectionbuf ,N_T("SampleClk")		,16368000	,FilePath);
	m_SampleOffset	= GetPrivateProfileInt(sectionbuf ,N_T("SampleOffset")	,0			,FilePath);
	m_AccumPeriod	= GetPrivateProfileInt(sectionbuf ,N_T("AccumPeriod")	,2			,FilePath);

	wcscpy(sectionbuf ,N_T("SEARCH"));
	m_CarrFreqCenter= GetPrivateProfileInt(sectionbuf ,N_T("CarrerCenter")		,4092000,FilePath);
	m_CarrStepWidth	= GetPrivateProfileInt(sectionbuf ,N_T("CarrerStepWidth")	,500	,FilePath);
	m_CarrStepN		= GetPrivateProfileInt(sectionbuf ,N_T("CarrerStepN")		,32		,FilePath);
	m_CodePhaseReso	= GetPrivateProfileInt(sectionbuf ,N_T("CodePhaseReso")		,2		,FilePath);

	wcscpy(sectionbuf ,N_T("FINE"));
	m_FineCarrStepWidth = GetPrivateProfileInt(sectionbuf ,N_T("CarrerStepWidth"),50	,FilePath);
	m_FineCarrStepN	=  GetPrivateProfileInt(sectionbuf ,N_T("CarrerStepNum")	,64		,FilePath);
	m_FineCodePhaseReso = GetPrivateProfileInt(sectionbuf ,N_T("FineCodeReso"),16	,FilePath);
	m_FineCodeStepN	= GetPrivateProfileInt(sectionbuf ,N_T("CodeStepNum")		,64		,FilePath);

	UpdateData(FALSE);

	return 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 他スレッドからデータ受けた場合
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
LRESULT CGpsL1SearchDlg::OnSearchCompl(WPARAM wParam, LPARAM lParam)
{
	static int32_t mabiki_co = 0;

	UpdateData(FALSE);

	return 0; // メッセージ固有の戻り値を返す
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  ダイアログにファイルをD&Dされたときのハンドラー
//  ダイアログのAccept FilesをTrueに設定する。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CGpsL1SearchDlg::OnDropFiles(HDROP hDropInfo)
{
    //ドロップされたファイルの個数を取得
    //DragQueryFile の詳細についてはmsdn参照
    //第2引数に -1 を指定すると、DragQueryFile はドロップされたファイルの総数を返します
    //unsigned int型の 0 を反転させると -1(0xFFFFFFFF) です
    UINT iCnt = DragQueryFile(hDropInfo, ~0u, NULL, 0);

    for (UINT i = 0; i<iCnt; i++)
    {
        //ファイル名の長さを取得
        //第3引数にNULLを指定すると、DragQueryFile は必要なバッファのサイズを文字単位で返します
        UINT iLen = DragQueryFile(hDropInfo, i, NULL, 0);
        //ファイル名を取得
        //第2引数に0～ドロップされたファイルの総数未満のいずれかの値を指定すると、その値に対応するファイル（フォルダ）名を格納します
        CString csfile;
        DragQueryFile(hDropInfo, i, csfile.GetBuffer(iLen + 1), iLen + 1);
        csfile.ReleaseBuffer();

        //ファイル（フォルダ）名を保持
		AfxMessageBox(csfile);
        //エディットボックスにパスを表示
        //複数ファイルドロップした場合は結果として最後に処理したパスが表示される）
    }

	CDialogEx::OnDropFiles(hDropInfo);
}

void CGpsL1SearchDlg::OnBnClickedButtonCudaInfo()
{
	CCudaInfoDlg dlg;

	dlg.DoModal();
}

void CGpsL1SearchDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	setStatusBar(*this);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Open GL 開始 ボタンクリック
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CGpsL1SearchDlg::OnBnClickedButtonGl()
{
	UpdateData(TRUE);

	if(m_IfFilePath.IsEmpty()){
		MessageBox(N_T("IFファイルを読み込んで下さい。"));
		return;
	}

	dump16ToUint32(m_DetectCodePhase ,m_DetectCodePhasei);

	m_SearchMode	= 0;


	if(gGLThread.isQuit()){
		gGLThread.m_PramTrig = 1;
		gGLThread.start();
	}else{
		gGLThread.m_PramTrig = 1;
	}
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FINE ボタン押された。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CGpsL1SearchDlg::OnBnClickedButtonGlFine()
{
	UpdateData(TRUE);

	if(m_IfFilePath.IsEmpty()){
		MessageBox(N_T("IFファイルを読み込んで下さい。"));
		return;
	}
	
	dump16ToUint32(m_DetectCodePhase ,m_DetectCodePhasei);

	m_SearchMode	= 1;

	if(gGLThread.isQuit()){
		gGLThread.m_PramTrig = 1;
		gGLThread.start();
	}else{
		gGLThread.m_PramTrig = 1;
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// IFファイルボタン押された。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CGpsL1SearchDlg::OnBnClickedButtonSetIfFilepath()
{
	CFileDialog dlgFile(TRUE);
	OPENFILENAME &ofn = dlgFile.GetOFN();
	ofn.lpstrTitle = _T("IFファイルを開く");
	ofn.lpstrFilter = _T("All Files\0*.*\0IF File\0*.dat\0Log Files\0*.log\0");
	ofn.Flags |= OFN_SHOWHELP;

	UpdateData(TRUE);


	if(dlgFile.DoModal()==IDOK){
		try{

			cudaLoad(gIfData , static_cast<std::wstring>(dlgFile.GetPathName().GetString()));	// CString → wstring変換

			m_IfFilePath = dlgFile.GetPathName();
			UpdateData(FALSE);

		}catch(std::exception e){
			MessageBox(CString(e.what()));
			return;
		}
	}
}

