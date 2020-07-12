// CudaInfoDlg.cpp : 実装ファイル
//


//#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"

#include "stdafx.h"

#include "GnrlCharset.h"

#include "SignalProcDlg.h"
#include "CudaInfoDlg.h"
#include "afxdialogex.h"


// CUDA runtime
#include <cuda_runtime.h>
// helper functions and utilities to work with CUDA
//#include <helper_functions.h>

//#include <helper_cuda.h>


void getCudaDev();
CString getCudaDevA();

// CCudaInfoDlg ダイアログ

IMPLEMENT_DYNAMIC(CCudaInfoDlg, CDialog)

CCudaInfoDlg::CCudaInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCudaInfoDlg::IDD, pParent)
	, m_DevicePropertie(_T(""))
{
//	getCudaDev();
	m_DevicePropertie = getCudaDevA();
}

CCudaInfoDlg::~CCudaInfoDlg()
{
}

void CCudaInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DEVICE_PROPERTIE, m_DevicePropertie);

}


BEGIN_MESSAGE_MAP(CCudaInfoDlg, CDialog)
END_MESSAGE_MAP()


// *********************************************************************************
//  CUDA情報を取得して文字に変換する
// *********************************************************************************
CString getCudaDevA()
{

    int i,ack,n=0;    //デバイス数
	CString ret;
	char str[256];

	ack = cudaGetDevice(&n);
 
	if ((ack != 0) || (n < 0)){
		return _T("Device not found\r\n");
	}

    for(i = 0; i <= n; i++){
        cudaDeviceProp dev;
 
        // デバイスプロパティ取得
//        cutilSafeCall(cudaGetDeviceProperties(&dev, i));
		cudaGetDeviceProperties(&dev, i);

//--		wsprintf(str, N_T("device %d\r\n"), i);
		
		sprintf_s(str, sizeof(str), "device %d\r\n", i);
		ret += str;
		sprintf_s(str, sizeof(str), " device name : %s\r\n", dev.name);
		ret += str;
		sprintf_s(str, sizeof(str), " total global memory : %d (MB)\r\n", dev.totalGlobalMem / 1024 / 1024);
		ret += str;
		sprintf_s(str, sizeof(str), " shared memory / block : %d (KB)\r\n", dev.sharedMemPerBlock / 1024);
		ret += str;
		sprintf_s(str, sizeof(str), " register / block : %d\r\n", dev.regsPerBlock);
		ret += str;
		sprintf_s(str, sizeof(str), " warp size : %d\r\n", dev.warpSize);
		ret += str;
		sprintf_s(str, sizeof(str), " max pitch : %d (B)\r\n", dev.memPitch);
		ret += str;
		sprintf_s(str, sizeof(str), " max threads / block : %d\r\n", dev.maxThreadsPerBlock);
		ret += str;
		sprintf_s(str, sizeof(str), " max size of each dim. of block : (%d, %d, %d)\r\n", dev.maxThreadsDim[0], dev.maxThreadsDim[1], dev.maxThreadsDim[2]);
		ret += str;
		sprintf_s(str, sizeof(str), " max size of each dim. of grid  : (%d, %d, %d)\r\n", dev.maxGridSize[0], dev.maxGridSize[1], dev.maxGridSize[2]);
		ret += str;
		sprintf_s(str, sizeof(str), " clock rate : %d (MHz)\r\n", dev.clockRate / 1000);
		ret += str;
		sprintf_s(str, sizeof(str), " total constant memory : %d (KB)\r\n", dev.totalConstMem / 1024);
		ret += str;
		sprintf_s(str, sizeof(str), " compute capability : %d.%d\r\n", dev.major, dev.minor);
		ret += str;
		sprintf_s(str, sizeof(str), " alignment requirement for texture : %d\r\n", dev.textureAlignment);
		ret += str;
		sprintf_s(str, sizeof(str), " device overlap : %s\r\n", (dev.deviceOverlap ? "ok" : "not"));
		ret += str;
		sprintf_s(str, sizeof(str), " num. of multiprocessors : %d\r\n", dev.multiProcessorCount);
		ret += str;
		sprintf_s(str, sizeof(str), " kernel execution timeout : %s\r\n", (dev.kernelExecTimeoutEnabled ? "on" : "off"));
		ret += str;
		sprintf_s(str, sizeof(str), " integrated : %s\r\n", (dev.integrated ? "on" : "off"));
		ret += str;
		sprintf_s(str, sizeof(str), " host memory mapping : %s\r\n", (dev.canMapHostMemory ? "on" : "off"));
 
		ret += N_T(" compute mode : ");
        if(dev.computeMode == cudaComputeModeDefault){
			sprintf_s(str, sizeof(str), "default mode (multiple threads can use) \r\n");
		}else if(dev.computeMode == cudaComputeModeExclusive){
			sprintf_s(str, sizeof(str), "exclusive mode (only one thread will be able to use)\r\n");
        }else if(dev.computeMode == cudaComputeModeProhibited){
			sprintf_s(str, sizeof(str), "prohibited mode (no threads can use)\r\n");
        }
		ret += str;
		
    }

	return ret;
}

// CCudaInfoDlg メッセージ ハンドラー
