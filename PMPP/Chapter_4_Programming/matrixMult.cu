#include<iostream>
#include"../cuda_note.h"
#include"../cuda_debug.h"
#define ROW_LEFT 512 
#define COL_RIGHT 256
#define K 1000

int main(){
    int sizeMatrixLeft = sizeof(float) * ROW_LEFT * K;
    int sizeMatrixRight = sizeof(float) * K * COL_RIGHT;
    int sizeMatrixOutput = sizeof(float) * ROW_LEFT * COL_RIGHT; 
    float *h_matrixLeft     =   (float *) malloc(sizeMatrixLeft);
    float *h_matrixRight    =   (float *) malloc(sizeMatrixRight);
    float *h_matrixOutput         =   (float *) malloc(sizeMatrixOutput);
    initial2DMatrix<float>(h_matrixLeft, ROW_LEFT, COL_RIGHT, 1);
    initial2DMatrix<float>(h_matrixRight, ROW_LEFT, COL_RIGHT, 1);
    float *d_matrixLeft, *d_matrixRight, *d_matrixOutput;
    cudaError_t err;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    err = cudaMalloc(&d_matrixLeft, sizeMatrixLeft);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMemcpy(d_matrixLeft, h_matrixLeft, sizeMatrixLeft, cudaMemcpyHostToDevice);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMalloc(&d_matrixRight, sizeMatrixRight);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMemcpy(d_matrixRight, h_matrixRight, sizeMatrixRight, cudaMemcpyHostToDevice);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMalloc(&d_matrixOutput, sizeMatrixOutput);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }

    /* 
        Invoke Simple MatrixMult Kernel
    */
    dim3 grid(ceil(ROW_LEFT/32), ceil(COL_RIGHT/32), 1);
    dim3 block(32, 32 ,1);
    cudaEventRecord(start);
    MatrixMatrixMultTiled<<<grid, block>>>(d_matrixLeft, d_matrixRight, d_matrixOutput);
    //MatrixMatrixMult<<<grid, block>>>(d_matrixLeft, d_matrixRight, d_matrixOutput);
    cudaEventRecord(stop);
    err = cudaMemcpy(h_matrixOutput, d_matrixOutput, sizeMatrixOutput, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);
    float ms = 0;
    cudaEventElapsedTime(&ms, start, stop);
    printf("Kernel execution time is %f ms \n",  ms);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    cudaFree(d_matrixLeft);
    cudaFree(d_matrixRight);
    cudaFree(d_matrixOutput);
    checkMatrixMatrixMult<float>(h_matrixLeft, h_matrixRight, h_matrixOutput, ROW_LEFT, K, K, COL_RIGHT);
    free(h_matrixLeft);
    free(h_matrixRight);
    free(h_matrixOutput);
}