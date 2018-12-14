#include<iostream>
#include"../cuda_note.h"
#include"../cuda_debug.h"
#define ROW 1024
#define COL 1024

int main(){
    int sizeMatrix = sizeof(float) * ROW * COL;
    int sizeVector = sizeof(float) * ROW;
    float *h_matrixInput    =   (float *) malloc(sizeMatrix);
    float *h_vectorInput    =   (float *) malloc(sizeVector);
    float *h_output         =   (float *) malloc(sizeVector);
    float *d_matrixInput, *d_vectorInput, *d_output;
    initial2DMatrix<float>(h_matrixInput, ROW, COL);
    initial2DMatrix<float>(h_vectorInput, 1, COL);
    cudaMalloc(&d_matrixInput, sizeMatrix);
    cudaMalloc(&d_vectorInput, sizeVector);
    cudaMalloc(&d_output, sizeVector);
    cudaMemcpy(d_matrixInput, h_matrixInput, sizeMatrix, cudaMemcpyHostToDevice);
    cudaMemcpy(d_vectorInput, h_vectorInput, sizeVector, cudaMemcpyHostToDevice);
    
    matrixVectorMult<<<ceil(ROW/256), 256>>>(d_matrixInput, d_vectorInput, d_output, COL, ROW);
    cudaMemcpy(h_output, d_output, sizeVector, cudaMemcpyDeviceToHost);
    cudaFree(d_matrixInput);
    cudaFree(d_vectorInput);
    cudaFree(d_output);
    
    checkMatrixVectorMult<float>(h_matrixInput, h_vectorInput, h_output, ROW, COL);

}