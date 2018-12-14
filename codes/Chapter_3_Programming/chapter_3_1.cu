#include<iostream>
#include"../cuda_note.h"
#include"../cuda_debug.h"
#define ROW 1024
#define COL 1024

/*Chapter 3 Problem 1*/
int main(int argc, char* argv[]){
    int size = sizeof(float) * ROW * COL;
    float *h_firstInput     =   (float *) malloc(size);
    float *h_secondInput    =   (float *) malloc(size);
    float *h_output         =   (float *) malloc(size);
    initial2DMatrix<float>(h_firstInput, ROW, COL, 1);
    initial2DMatrix<float>(h_secondInput, ROW, COL, 1);
    float *d_firstInput, *d_secondInput, *d_output;
    cudaError_t err;
    int debug = 0;
    if (debug){ 
      peakMatrix<float> (h_firstInput, ROW, COL);
      peakMatrix<float> (h_secondInput, ROW, COL);
    } 
    
    
    err = cudaMalloc(&d_firstInput, size);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMemcpy(d_firstInput, h_firstInput, size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMalloc(&d_secondInput, size);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMemcpy(d_secondInput, h_secondInput, size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    err = cudaMalloc(&d_output, size);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    /*Chapter 3 problem 1.a --> matrixAdd*/
    //matrixAdd <<<ceil(ROW * COL/512), 512>>> (d_firstInput, d_secondInput, d_output, ROW * COL);
    /*Chapter 3 problem 1.b --> matrixAddRow*/
    //matrixAddRow <<<ceil(ROW/ 512), 512>>> (d_firstInput, d_secondInput, d_output, ROW * COL, COL);
    /*Chapter 3 problem 1.c --> matrixAddCol*/
    matrixAddRow <<<ceil(COL/ 512), 512>>> (d_firstInput, d_secondInput, d_output, ROW * COL, ROW);

    err = cudaMemcpy(h_output, d_output, size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess){
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__, __LINE__);
    }
    cudaFree(d_firstInput);
    cudaFree(d_secondInput);
    cudaFree(d_output);
    checkAdditionResult<float>(h_firstInput, h_secondInput, h_output, ROW, COL);
    free(h_firstInput);
    free(h_secondInput);
    free(h_output);
}