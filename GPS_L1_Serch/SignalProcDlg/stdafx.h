
// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。

#pragma once

#define _CRT_SECURE_NO_WARNINGS


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows ヘッダーから使用されていない部分を除外します。
#endif

#include "targetver.h"


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 一部の CString コンストラクターは明示的です。

// 一般的で無視しても安全な MFC の警告メッセージの一部の非表示を解除します。
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分


#include <afxdisp.h>        // MFC オートメーション クラス



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC におけるリボンとコントロール バーのサポート









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



// **** OPENGL ****
#define _SCL_SECURE_NO_WARNINGS
//#pragma warning(disable:4996)

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

// includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <helper_gl.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <cufft.h>

#include <helper_cuda.h>
#include <helper_functions.h>
#include <math_constants.h>

#if defined(__APPLE__) || defined(MACOSX)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <rendercheck_gl.h>		// MIN,MAXマクロあるとコンパイルエラーになる。
// **** OPENGL ****





#include <stdint.h>

#include "GnrlString.h"

#define USE_MFC

class ComRecvThread;
class GLThread;



#include <iostream>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>

#include "L1PrnTap.h"
#include "EditInListCtrl.h"

#include "GnrlInt.h"
#include "GnrlCharset.h"
#include "GnrlDefine.h"


#include "SP_Define.h"
#include "SP_Global.h"



#include "GLThread.h"





