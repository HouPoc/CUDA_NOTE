#include<cuda.h>
#include<string>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include"cn_data_structure.h"
#define COMMON_WIDTH 512
#define ROW_LEFT 500 
#define COL_RIGHT 250
#define K 1000
#define TILE_WIDTH 32
__device__ int D_ROW_LEFT = ROW_LEFT;
__device__ int D_COL_RIGHT = COL_RIGHT;
__device__ int D_K = K;


using namespace std;

typedef class dev_limit dev_limit;




/*
  print out current platform infomation
  return device pointer
  store device grid limitation and block limitation
*/
void platformInfo(int *dev_count, bool dev_info =true){
  cudaError_t err;
  //int a = 0;
  printf("Device Count: %d\n", *dev_count);
  err = cudaGetDeviceCount(dev_count);
  if (err !=cudaSuccess){
    printf("platform_info error :  %s \n", cudaGetErrorString(err));
    exit(-1);
  }
  else {
    if (dev_info){
      cudaDeviceProp dev_prop;
      for (int i = 0; i < *dev_count; i++){
        cudaGetDeviceProperties(&dev_prop, i);
        printf("Device Number: %d\n", i);
        printf("\t Device Name: %s. \n", dev_prop.name);
        printf("\t Memory Clock Rate (GHz): %f\n", (dev_prop.memoryClockRate/1.0e6));
        printf("\t Memory Bus Width (bits): %d\n", dev_prop.memoryBusWidth);
        printf("\t Peak Memory Bandwidth (GB/s): %f\n",2.0*dev_prop.memoryClockRate*(dev_prop.memoryBusWidth/8)/1.0e6);
        printf("\t Shared Memory Per Block (Bytes): %zu\n" , dev_prop.sharedMemPerBlock);
        dev_limit *new_dev = new dev_limit(dev_prop.maxGridSize, dev_prop.maxThreadsDim);
        printf("\t Grid Dimention:  (%d, %d, %d) \n", new_dev->grid->x, new_dev->grid->y, new_dev->grid->z);
        printf("\t Block Dimention: (%d, %d, %d) \n", new_dev->block->x, new_dev->block->y, new_dev->block->z);
        
      }
    }
  }
}


/*
  dynamic allocate 2D Matrix for given data type
*/

template <class T>
void initial2DMatrix (T* matrix, int row, int col ,int random = 0){
    srand (time(NULL));
    for (int i = 0; i < row; i++){
        for (int j= 0; j < col; j++){ 
            matrix[i * row + j] = (T) (random? 20.0 *((float) rand()) / (float) RAND_MAX : 0);
        }
    }
}



/*
    Problem 3.1.b
    In this kernel, each thread produces one cell of output matrix
*/
__global__
void matrixAdd(float *firstInput, float *secondInput, float* outPut, int n){
    int realIdx = blockDim.x * blockIdx.x + threadIdx.x;
    if (realIdx < n){
        outPut[realIdx] = firstInput[realIdx] + secondInput[realIdx];
    }
}
/*
   problem 3.1.c
   In this kernel, each thread produces one row of the output matrix
*/
__global__
void matrixAddRow(float *firstInput, float *secondInput, float *outPut, int n, int COL){
    int rowIdx = blockDim.x * blockIdx.x + threadIdx.x;
    for (int i = 0; i < COL; i++){
        if ((rowIdx * COL+ i) < n){
            outPut[rowIdx * COL + i] = firstInput[rowIdx * COL + i] + secondInput[rowIdx * COL + i];
        }
    }
}

/*
   problem 3.1.d
   In this kernel, each thread produces one column of the output matrix
*/
__global__
void matrixAddCol(float *firstInput, float *secondInput, float *outPut, int n, int ROW){
    int colIdx = blockDim.x * blockIdx.x + threadIdx.x;
    for (int i = 0; i < ROW; i++){
        if ((colIdx * ROW + i) < n){
            outPut[colIdx * ROW + i] = firstInput[colIdx * ROW + i] + secondInput[colIdx * ROW + i];
        }
    }
}

/*
   problem 3.2
   Matrix Vector Mult 
   In this kernel, each thread will calculate one element in the result vector   
*/
__global__
void matrixVectorMult(float *matrix, float *vector, float *output, int COL, int ROW)
{
   // Idx'th element in result ----> Idx'th row of matrix* vector  
   int Idx = blockDim.x * blockIdx.x + threadIdx.x;
  
   if (Idx < COL) {
        float tmp = 0.0;
        for (int i = 0 ; i < COL; i++){
            tmp += matrix[Idx* ROW + i] * vector[i];
        }
        output[Idx] = tmp;
   }
}
/*
    Chapter 4 Practice
    Matrix Matrix Mult
    In this kernel, each thread will calculate one element of the final matrix
    K = first matrix's col num = second matrix's row num
    D_ROW_LEFT = first matrix's row num
    D_COL_RIGHT = second matrix's col num 
    newMatrix = D_ROW_LEFT x D_COL_RIGHT
*/
__global__
void MatrixMatrixMult(float *matrixLeft, float *matrixRight, float *output){
    int col = blockDim.x * blockIdx.x + threadIdx.x;
    int row = blockDim.y * blockIdx.y + threadIdx.y; 
    if ( row < D_ROW_LEFT && col < D_COL_RIGHT){
        float value = 0.0;
        for (int i = 0 ; i < D_K; i++){
            value += matrixLeft[row * D_K + i] * matrixRight[i * D_K + col];
        }
        output[row * D_K + col] = value;
    }  
}

/*
    Chapter 4 Practice
    Matrix Matrix Mult
    In this Kernel, we use tiled matrix mult algoritm and static shared memory. Each thread will 
    calculate one element of the final matrix
    K = first matrix's col num = second matrix's row num
    D_ROW_LEFT = first matrix's row num
    D_COL_RIGHT = second matrix's col num
    TILE_WIDTH = the TILE matrix's size
    newMatrix = D_ROW_LEFT x D_COL_RIGHT
*/
__global__
void MatrixMatrixMultTiled(float *matrixLeft, float *matrixRight, float *output){
    __shared__  float sMatrixLeft[TILE_WIDTH][TILE_WIDTH];
    __shared__  float sMatrixRight[TILE_WIDTH][TILE_WIDTH];  
   int bx = blockIdx.x; int by = blockIdx.y;
   int tx = threadIdx.x; int ty = threadIdx.y;
   int col = bx * TILE_WIDTH + tx;
   int row = by * TILE_WIDTH + ty;
   float value = 0;
   for (int i = 0; i < ceil(D_K/(float)TILE_WIDTH); ++i){
       if (row < D_ROW_LEFT && row * D_K + i * TILE_WIDTH  +tx < D_K){
        sMatrixLeft[ty][tx]  = matrixLeft[row * D_K + i * TILE_WIDTH  +tx];
       }
       if (col < D_COL_RIGHT && (ty + i * TILE_WIDTH) * D_COL_RIGHT  + col < D_K ){
        sMatrixRight[ty][tx] = matrixRight[(ty + i * TILE_WIDTH) * D_COL_RIGHT  + col];
       }
       __syncthreads();
       for (int j = 0; j < TILE_WIDTH; j++){
           value += sMatrixLeft[ty][j] * sMatrixRight[j][tx]; 
       }
       if (row < D_ROW_LEFT && col < D_COL_RIGHT ){
        output[row * D_COL_RIGHT + col] = value;
       }
   }
}


/*
    Chapter 5 Practice
    Sum Reduction 
    In this kernel, we use reduction algorithm to calculate the sum of an array
*/
__global__
void sumReduction(float *input, float *output, int len){
    __shared__ float partialSum[COMMON_WIDTH];
    int t = threadIdx.x;
    int globalId = blockIdx.x * blockDim.x + threadIdx.x;
    if (globalId< len){
        partialSum[t] = input[globalId];
    }else{
        partialSum[t] = 0.0;
    }
    for (int stride = 1; stride < blockDim.x; stride*=2){
        __syncthreads();
        int index = 2 * stride * threadIdx.x;
        if (index < blockDim.x){
            partialSum[index] += partialSum[index + stride];
        }
    }
    __syncthreads();
    if (t==0){
        output[blockIdx.x] = partialSum[0];
    }
}

/*
    Chapter 5 Practice
    Sum Reduction 
    In this kernel, we use reduction algorithm to calculate the sum of an array but reduce brach
    divergency in warp
*/

__global__
void sumReductionNoBranch(float *input, float* output, int len){
    __shared__ float partialSum[COMMON_WIDTH];
    int t = threadIdx.x;
    int globalId = blockIdx.x * blockDim.x + threadIdx.x;
    if (globalId < len){
        partialSum[t] = input[globalId];
    }else{
        partialSum[t] = 0.0;
    } 
    for (int stride = blockDim.x / 2 ; stride >= 1; stride = stride >> 1){
        __syncthreads();
        if (t < stride){
            partialSum[t] += partialSum[t+stride];
        }
    }
    __syncthreads();
    if (t==0){
        output[blockIdx.x] = partialSum[0];
    }   
}

/*
    Chapter 5 Exercise 1 Solution 1
    In this problem, we are required to modify above two kernels to reduce idle threads 
    In the kernel without caring branch divergency, only half of the threads work from
    the first iteration. To elimate that waste, we can ask all threads to work in the 
    beginning.  
*/

/*  
        First, we modify the kernel without considersing branch divergence within warp.
        We load two elements at the beginning. We load elements from two adjacent blocks
        to take advantage of memory coalescing.
            
*/
__global__
void sumReductionModify(float *input, float *output, int len){
    __shared__ float partialSum[COMMON_WIDTH];
    int tId = threadIdx.x;
    int globalId = blockIdx.x * (blockDim.x * 2) + threadIdx.x;
    if (globalId < len){
        partialSum[tId] = input[globalId];
    }
    if (globalId + blockDim.x < len) {
        partialSum[tId] += input[globalId + blockDim.x];
    }
    __syncthreads();
    for (int stride = 1; stride < blockDim.x; stride*=2){
        __syncthreads();
        int index = 2 * stride * threadIdx.x;
        if (index < blockDim.x){
            partialSum[index] += partialSum[index + stride];
        }
    }
    __syncthreads();
    if (tId==0){
        output[blockIdx.x] = partialSum[0];
    } 
}
/*
        Second, we modify the kernel without branch divergence. 
*/
__global__
void sumReductionNoBranchModify(float *input, float* output, int len){
    __shared__ float partialSum[COMMON_WIDTH];
    int tId = threadIdx.x;
    int globalId = blockIdx.x * (blockDim.x * 2)+ threadIdx.x;
    if (globalId < len){
        partialSum[tId] = input[globalId];
    }
    if (globalId + blockDim.x < len) {
        partialSum[tId] += input[globalId + blockDim.x];
    }
    for (int stride = blockDim.x / 2 ; stride >= 1; stride = stride >> 1){
        __syncthreads();
        if (tId < stride){
            partialSum[tId] += partialSum[tId+stride];
        }
    }
    __syncthreads();
    if (tId==0){
        output[blockIdx.x] = partialSum[0];
    }   
}