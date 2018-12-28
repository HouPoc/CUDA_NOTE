#include<iostream>
#include"../cuda_note.h"
#include"../cuda_debug.h"
#define LEN 400000

void sumReduction(){
    
}

int main(){
    enum RD {   
                reductionWithBranch = 1,
                reductionWithNoBranch = 2,
                reductionWithBranchNoIdleThread = 3,
                reductionWithNoBranchNoIdleThread = 4
            };
    RD reductionStrategy = reductionWithNoBranchNoIdleThread; 

   
    int numBlocks;
    int inputSize = sizeof(float) * LEN;
    if (reductionStrategy == reductionWithBranch 
     || reductionStrategy == reductionWithNoBranch)
    {
        numBlocks = ceil((float)LEN/(float)COMMON_WIDTH);
          
    }
    if (reductionStrategy == reductionWithBranchNoIdleThread 
     || reductionStrategy == reductionWithNoBranchNoIdleThread)
    {
        numBlocks = ceil((float)LEN/(float)(2 * COMMON_WIDTH));   
    }
    dim3 block(COMMON_WIDTH, 1, 1);
    dim3 grid(numBlocks, 1, 1); 
    
    float ms;

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
    switch (reductionStrategy){
        case reductionWithBranch: 
            printf("reductionWithBranch \n");
            sumReduction<<<grid, block>>>(d_input, d_output, LEN); break;
        case reductionWithNoBranch:
            printf("reductionWithNoBranch \n");
            sumReductionNoBranch<<<grid, block>>>(d_input, d_output, LEN); break;
        case reductionWithBranchNoIdleThread:
            printf("reductionWithBranchNoIdleThread \n");
            sumReductionModify<<<grid, block>>>(d_input, d_output, LEN); break;
        case reductionWithNoBranchNoIdleThread: 
            printf("reductionWithNoBranchNoIdleThread \n");
            sumReductionNoBranchModify<<<grid, block>>>(d_input, d_output, LEN); break;
    }
    cudaEventRecord(stop);

    cudaMemcpy(h_input, d_input, inputSize, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_output, d_output, sizeof(float) * numBlocks, cudaMemcpyDeviceToHost);
    
    cudaEventSynchronize(stop);
    
    cudaEventElapsedTime(&ms, start, stop);
    printf("Kernel execution time is %f ms \n",  ms);
    checkSumReduction<float>(h_input, h_output, LEN, numBlocks);
    cudaFree(d_input);
    free(h_input);

}