// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// CudaMem.h
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#ifndef CUDAMEM_H
#define CUDAMEM_H

//#include "stdint.h"

// System includes
//#include <stdio.h>
//#include <assert.h>

// CUDA runtime
//#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
//#include <helper_functions.h>
//#include <helper_cuda.h>

struct CudaMemInt8;
extern CudaMemInt8 gIfData;	// A/DÇµÇΩIFÉfÅ[É^

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
///template<typename X>
struct CudaMemInt8{

	CudaMemInt8();
	~CudaMemInt8();

	cudaError_t memalloc(size_t Len);
	void memFree();
	cudaError_t putMemCopy(size_t DstOffset ,const int8_t *SrcData ,size_t Len);
	cudaError_t getMemCopy(size_t SrcOffset ,int8_t *DstData ,size_t Len) const;
	operator int8_t*();

	int8_t	*m_Data;
	size_t	m_Len;
};


//class CString;
int32_t cudaLoad(CudaMemInt8 &CudaMem ,const std::wstring &FilePath);


#endif // CUDAMEM_H

