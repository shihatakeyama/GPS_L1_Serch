// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GL_.cu
//
// OpenGL‚ğg—p‚·‚é‚Ì‚É’Ç‰Á‚·‚éƒ‰ƒCƒuƒ‰ƒŠ
// glew64.lib
// C:\ProgramData\NVIDIA Corporation\CUDA Samples\v10.0\common\lib\x64
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include <cufft.h>
#include <math_constants.h>

#include "GPU_helper.h"


// L1 M†ˆ—
__global__ void generateSpectrumKernel(float2 *h0,
                                       float2 *ht,
                                       unsigned int in_width,
                                       unsigned int out_width,
                                       unsigned int out_height,
                                       float t,
                                       float patchSize)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
    unsigned int in_index = y*in_width+x;
    unsigned int in_mindex = (out_height - y)*in_width + (out_width - x); // mirrored
    unsigned int out_index = y*out_width+x;

}

// update height map values based on output of FFT
__global__ void updateHeightmapKernel(float  *heightMap,
                                      float2 *ht,
                                      unsigned int width,
									  float thetaco)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
    unsigned int i = y*width+x;

#if 0
    // cos(pi * (m1 + m2))
    float sign_correction = ((x + y) & 0x01) ? -1.0f : 1.0f;

    heightMap[i] = ht[i].x * sign_correction;
#endif

#if 0
	heightMap[i] = sin((x + y)*0.1f)*0.1f;
#endif
#if 1
	heightMap[i] = sin((float)x * 0.2f) * sin((float)y*0.2f + thetaco) * 5.2f;
#endif

}
// ‚‚³‚ğZo
extern "C"
void cudaUpdateHeightmapKernel(float  *d_heightMap,
                               float2 *d_ht,
                               unsigned int width,
                               unsigned int height,
                               bool autoTest)
{
	static float thetaco = 0.0f;

    dim3 block(16, 16, 1);
    dim3 grid(cuda_iDivUp(width, block.x), cuda_iDivUp(height, block.y), 1);

	updateHeightmapKernel <<<grid, block >>>(d_heightMap, d_ht, width, thetaco);

	thetaco = thetaco + 0.06f;
	if (thetaco > (CUDART_PI_F * 2)){
		thetaco -= (CUDART_PI_F * 2);
	}
}

// ‰A‰e‚ğŒvZ‚·‚éB
// generate slope by partial differences in spatial domain
__global__ void calculateSlopeKernel(float2 *slopeOut, const float *h, unsigned int width, unsigned int height ,float Sensitivity)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
    unsigned int i = y*width+x;

    float2 slope = make_float2(0.0f, 0.0f);

    if ((x > 0) && (y > 0) && (x < width-1) && (y < height-1)){
		slope.x= (h[i + 1] - h[i - 1]) *Sensitivity;			// 0.002f;				//	/ h[i]/1024;	// /64;
		slope.y = (h[i + width] - h[i - width]) * Sensitivity;	//0.002f;	// /64;
    }

    slopeOut[i] = slope;
}

// ŒXÎ‚ğZo
extern "C"
void cudaCalculateSlopeKernel(float *hptr, float2 *slopeOut,
                              unsigned int width, unsigned int height  ,float Sensitivity)
{
    dim3 block(8, 8, 1);
    dim3 grid2(cuda_iDivUp(width, block.x), cuda_iDivUp(height, block.y), 1);

    calculateSlopeKernel<<<grid2, block>>>(slopeOut, hptr, width, height ,Sensitivity);
}
