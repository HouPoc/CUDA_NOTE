#include <iostream>
#include "../cuda_debug.h"
#include "../cuda_note.h"


#define LEN 2000    // define the size of input/output data
#define BLOCK_DIM 256


int main(){
    size_t maskSize = sizeof(float) * MASK_WIDTH;
    size_t dataSize = sizeof(float) * LEN;
    float *mask =   (float *) malloc(maskSize);
    float *h_input =  (float *) malloc(dataSize);
    float *h_output = (float *) malloc(dataSize);
    initial2DMatrix<float>(h_input,1, LEN, 0);
    initial2DMatrix<float>(mask, 1, MASK_WIDTH, 0);
    float *d_input, *d_output;
   
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    
    cudaMalloc(&d_input, dataSize);
    cudaMalloc(&d_output, dataSize);
    cudaMemcpy(d_input, h_input, dataSize, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(d_mask, mask, maskSize);

    dim3 block(BLOCK_DIM);
    dim3 grid(ceil(LEN/(float)BLOCK_DIM));
    cudaEventRecord(start);
    convolution1D<<<grid, block>>>(d_input, d_output, LEN, MASK_WIDTH);
    cudaEventSynchronize(stop);
    cudaMemcpy(h_output, d_output, dataSize, cudaMemcpyDeviceToHost);
    float ms = 0;
    cudaEventElapsedTime(&ms, start, stop);
    printf("Kernel execution time is %f ms \n",  ms);
    cudaFree(d_input);
    cudaFree(d_output);
    check1Dconvolution<float>(h_input, mask, h_output, LEN, MASK_WIDTH);
    free(h_input);
    free(h_output);
    free(mask);

    return 0;
}