// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  CudaMem.cpp
//
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"

// System includes
#include <stdio.h>
#include <iostream>
//#include <fstream>
#include <assert.h>

// CUDA runtime
#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
#include <helper_functions.h>
#include <helper_cuda.h>

#include <stdint.h>

#include "CudaMem.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//#include "CudaMem.h"
//__align__(4) 


CudaMemInt8 gIfData;	// A/D����IF�f�[�^


CudaMemInt8::CudaMemInt8()
:m_Data(nullptr),m_Len(0)
{}

CudaMemInt8::~CudaMemInt8()
{
	memFree();
}

cudaError_t CudaMemInt8::memalloc(size_t Len){
	cudaError_t cudaStatus;
	memFree();
	cudaStatus = cudaMalloc((void**)&m_Data, Len * sizeof(uint8_t));
	m_Len = Len;
	return cudaStatus;
}

void CudaMemInt8::memFree()
{
	if(m_Data!=nullptr)	cudaFree(m_Data);
	m_Data	= nullptr;
	m_Len	= 0;	
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �N���X�̊O��cuda�֐����g���悤�ɂ���̂��߂�ǂ������̂ŁB
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
cudaError_t CudaMemInt8::putMemCopy(size_t CudaOffset ,const int8_t *SrcData ,size_t Len)
{
	return cudaMemcpy(m_Data + CudaOffset ,SrcData ,Len ,cudaMemcpyHostToDevice);
}
cudaError_t CudaMemInt8::getMemCopy(size_t CudaOffset ,int8_t *DstData ,size_t Len) const
{
	return cudaMemcpy(DstData ,m_Data + CudaOffset ,Len ,cudaMemcpyDeviceToHost);
}

CudaMemInt8::operator int8_t*(){
	return m_Data;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �t�@�C����ǂݍ���ŁAGPU�̃O���[�o���������֊i�[����B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int32_t cudaLoad(CudaMemInt8 &CudaMem ,const std::wstring &FilePath)
{
	cudaError_t cudaStatus;
	const size_t maxstep = 1024*1024;		// �������T�C�Y�w��

	std::ifstream ifs(FilePath, std::ios::in | std::ios::binary );	// �o�C�i�����[�h�łɂ��Ȃ��ƁA�ǂݍ��񂾃t�@�C���T�C�Y������Ȃ��Ȃ�B
    if (ifs.fail()){
        return -3;
    }

    size_t begin = ifs.tellg();
    ifs.seekg(0, ifs.end);
    // �ꉞ�͈̓`�F�b�N���ׂ������ǁc�c
    size_t end = ifs.tellg();
    size_t size = end - begin;
    ifs.clear();		 // ������clear����EOF�t���O������
    ifs.seekg(0, ifs.beg);
    char *str = new char[maxstep];

	CudaMem.memalloc(size);

	size_t wco = 0;
	size_t stepsize;
	while(wco < size){

		stepsize = (size - wco);
		if(stepsize > maxstep)	stepsize = maxstep;

		ifs.read(str, stepsize);

		CudaMem.putMemCopy(wco ,reinterpret_cast<int8_t*>(str) ,stepsize);

		wco += stepsize;
	}

	delete str;

	return 0;
}