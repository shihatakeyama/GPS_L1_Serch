// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPU_helper.cpp
//
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// System includes
//#include <stdio.h>
#include <stdafx.h>
#include <assert.h>

// CUDA runtime
#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
#include <helper_functions.h>
#include <helper_cuda.h>


// Round a / b to nearest higher integer value
int cuda_iDivUp(int a, int b)
{
    return (a + (b - 1)) / b;
}

// アライメント a を b で高い方へアライメント
size_t cuda_alignUp(size_t a, size_t b)
{
    return ((a + (b - 1)) / b) * b;	
}


