// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPU_helper.h
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


// Round a / b to nearest higher integer value
int cuda_iDivUp(int a, int b);

// �A���C�����g a �� b �ō������փA���C�����g
size_t cuda_alignUp(size_t a, size_t b);

