// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GPU_helper.h
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


// Round a / b to nearest higher integer value
int cuda_iDivUp(int a, int b);

// アライメント a を b で高い方へアライメント
size_t cuda_alignUp(size_t a, size_t b);

