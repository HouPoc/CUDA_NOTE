#include<iostream>
#include"../cuda_note.h"
#include"../cuda_debug.h"
#define LEN 20000



int main(){
    int inputSize = sizeof(float) * LEN;
    int numBlocks = ceil((float)LEN/(float)COMMON_WIDTH);
    dim3 grid = (numBlocks, 1, 1);
    dim3 block = (COMMON_WIDTH, 1, 1);

    float *h_input = (float*)malloc(inputSize);
    float *h_output = (float *)malloc(sizeof(float) * numBlocks);
    float *d_input;
    float *d_output;
    
    initial2DMatrix<float>(h_input, 1, LEN, 1);
    

    cudaEvent_t start;
    cudaEvent_t stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    
    cudaMalloc(&d_input, inputSize);
    cudaMemcpy(d_input, h_input, inputSize, cudaMemcpyHostToDevice);

    cudaMalloc(&d_output, sizeof(float)* numBlocks);

    cudaEventRecord(start);
    sumReduction<<<grid, block>>>(float* input, float *output, int len, int stride);
    cudaEventRecord(stop);

    cudaMemcpy(h_input, d_input, inputSize, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_output, d_output, sizeof(float) * numBlocks, cudaMemcpyDeviceToHost);
    
    cudaEventSynchronize(stop);
    
    cudaEventElapsedTime(&ms, start, stop);
    printf("Kernel execution time is %f ms \n",  ms);
    
    cudaFree(d_input);
    free(h_input);

}