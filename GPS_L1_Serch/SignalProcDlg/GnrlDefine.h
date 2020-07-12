/***********************************************************************************
* Gnrlで必要なデファイン
* Gnrlをコンパイルするソースは、必ず本ファイルをインクルードして下さい。
* 
* GnrlDefine.h
*  (旧 GlobalDefine.h)
* 
* 2013/06/31  Createed
***********************************************************************************/

#ifndef GNRLDEFINE_H
#define GNRLDEFINE_H

// **** アプリケーションで使用するの機能に応じてデファインを活かして下さい。 ****
//#define USE_GNRLLIST
//#define USE_GNRLTABLE
#define USE_GNRLCONDITION
#define USE_GNRLBYTEORDER
//#define USE_GNRLEVENT
#define USE_STREAM_BUF


// **** 使用するPCの環境を定義してください ****
#define HAVE_CXX_VOLATILE	1		// コンパイラがvolatileに対応している場合
#define WINDOWS_INTERFACE	1		// WINDOWSで使用する場合

#undef MAX_PATH
#define MAX_PATH	260				// 環境に応じて値を増減して下さい

#define SIZE_OF_BUF	(1024*4)		// 文字列操作用バッファーサイズ

#if WINDOWS_INTERFACE==1
//#include "stdafx.h"				// Microsoft VisualC++使用の場合
#include <windows.h>
#endif

#define _CRT_SECURE_NO_WARNINGS 	// CRTの警告を出力しない
#pragma warning ( disable : 4996 ) 


// *********************************************************************************
// 各種マクロ
// *********************************************************************************
#if 0
// 大小判定マクロ ※VSで<stdio.h>をインクルードした後に定義するとエラーになる。
template <typename X> X MAX(X arg1, X arg2)	{ return arg1>arg2 ? arg1 : arg2; }
template <typename X> X MIN(X arg1, X arg2)	{ return arg1<arg2 ? arg1 : arg2; }
#endif


// 引数が任意の場合のマクロ
#define GNRL_VA_LIST		va_list
#define GNRL_VA_START(a,b)	va_start(a,b)
#define GNRL_VA_ARG(a,b)	va_arg(a,b)
#define GNRL_VA_END(a)		va_end (a)

// 引数が不定の呼び出し
//#define GNRL_VSPRINTF	_vstprintf	// TCHAR.H のルーチン
#define GNRL_VSPRINTF	vsprintf	// _UNICODE & _MBCS が未定義または_MBCS が定義されている場合
//#define GNRL_VSPRINTF	vswprintf	// _UNICODE が定義されている場合


#if 0
#define ZeroMemory(x,y) memset((x),0,(y))
#endif


// キャストは頻繁に呼び出す場合は、
// クラス内に専用メンバ関数を作成した方がよいかもしれない。

// ダウンキャストで使用してください。
#define GNRL_DOWN_CAST(To ,from){ assert(dynamic_cast<To>(from) == static_cast<To>(from) && "GNRL_STATIC_CAST 失敗");	return static_cast<To>(from);}

// フレーム解読など、ポインタの型変換で使用してください。
#define GNRL_REINTERPRET_CAST(t ,a) 	static_cast<t>((pVoid)a)


#endif // GNRLDEFINE


