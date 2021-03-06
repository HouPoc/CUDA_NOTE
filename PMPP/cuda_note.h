#include<cuda.h>
#include<string>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include"cn_data_structure.h"
#define COMMON_WIDTH 512
#define ROW_LEFT 5000
#define COL_RIGHT 2000
#define K 1000
#define TILE_WIDTH 32
#define MASK_WIDTH 5  // define the size of the mask
#define I_TILE_SIZE 32
#define O_TILE_SIZE (I_TILE_SIZE + 1 - MASK_WIDTH)
__device__ int D_ROW_LEFT = ROW_LEFT;
__device__ int D_COL_RIGHT = COL_RIGHT;
__device__ int D_K = K;
__constant__ float d_mask[MASK_WIDTH]; // allocate GPU constant memory  
__constant__ float d_mask2D[MASK_WIDTH][MASK_WIDTH];

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
        for (int j = 0; j < col; j++){ 
            matrix[i * col + j] = (T) (random? 20.0 *((float) rand()) / (float) RAND_MAX : 1);
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
   int col = bx * blockDim.x + tx;
   int row = by * blockDim.y + ty;
   float value = 0.0;
   for (int i = 0; i < ceil(D_K/(float)TILE_WIDTH); ++i){
       sMatrixLeft[ty][tx] = 0.0;
       sMatrixRight[ty][tx] = 0.0; 
       __syncthreads();
       if (row < D_ROW_LEFT && i * TILE_WIDTH + tx < D_K){
        sMatrixLeft[ty][tx]  = matrixLeft[row * D_K + i * TILE_WIDTH + tx];
       }
       if ((ty + i * TILE_WIDTH) < D_K && col < D_COL_RIGHT){
        sMatrixRight[ty][tx] = matrixRight[(ty + i * TILE_WIDTH) * D_COL_RIGHT  + col];
       }
       __syncthreads();

       for (int j = 0; j < TILE_WIDTH; j++){
           value += sMatrixLeft[ty][j] * sMatrixRight[j][tx]; 
       }
       __syncthreads();  
   }
   //output[0] = 0; 
   if (row < D_ROW_LEFT && col < D_COL_RIGHT ){
        output[row * D_COL_RIGHT + col] = value;
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
       
        if (t%(2*stride) == 0){
            partialSum[t] += partialSum[t + stride];
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
          
        if (tId%(2*stride) == 0){
            partialSum[tId] += partialSum[tId + stride];
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

/*
    Chapter 5 Problem 10 
    This kernel aims to increase the thread' granularity by processing 2
    adjacent elements at a time
*/

__global__
void matrixMultTiledIncreasedGranularity(float *matrixLeft, float *matrixRight, float *output){
    __shared__  float sMatrixLeft[TILE_WIDTH][TILE_WIDTH];
    __shared__  float sMatrixRight4FirstElement[TILE_WIDTH][TILE_WIDTH];  
    __shared__  float sMatrixRight4SecondElement[TILE_WIDTH][TILE_WIDTH];
    int bx = blockIdx.x * 2; int by = blockIdx.y;
    int tx = threadIdx.x; int ty = threadIdx.y;
    int col = bx * blockDim.x + tx;
    int row = by * blockDim.y + ty;
    float value4FirstElement = 0.0;
    float value4SecondElement = 0.0;
    for (int i = 0; i < ceil(D_K/ (float)(TILE_WIDTH)); i++){
        sMatrixLeft[ty][tx] = 0.0;
        sMatrixRight4FirstElement[ty][tx] = 0.0;
        sMatrixRight4SecondElement[ty][tx] = 0.0;
        __syncthreads(); 
        if (row < D_ROW_LEFT && i * TILE_WIDTH + tx < D_K){
            sMatrixLeft[ty][tx]  = matrixLeft[row * D_K + i * TILE_WIDTH + tx];
        }
        if ((ty + i * TILE_WIDTH) < D_K && col < D_COL_RIGHT){
        sMatrixRight4FirstElement[ty][tx] = matrixRight[(ty + i * TILE_WIDTH) * D_COL_RIGHT  + col];
        }
        if ((ty + i * TILE_WIDTH) < D_K && col + 1 < D_COL_RIGHT){
        sMatrixRight4SecondElement[ty][tx] = matrixRight[(ty + i * TILE_WIDTH) * D_COL_RIGHT  + col + TILE_WIDTH];
        }
        __syncthreads();
        for (int j = 0; j < TILE_WIDTH; j++){
           value4FirstElement += sMatrixLeft[ty][j] * sMatrixRight4FirstElement[j][tx]; 
           value4SecondElement += sMatrixLeft[ty][j] * sMatrixRight4SecondElement[j][tx];
        }
        __syncthreads();  
    }
   //output[0] = 0; 
    if (row < D_ROW_LEFT && col < D_COL_RIGHT ){
        output[row * D_COL_RIGHT + col] = value4FirstElement;
    }   
    if (row < D_ROW_LEFT && col + TILE_WIDTH < D_COL_RIGHT ){
        output[row * D_COL_RIGHT + col + TILE_WIDTH] = value4SecondElement;
    }  
}

/*
    In this kernel, we will performe basic 1D convolution with CUDA
*/
__global__ 
void convolution1D(float *input, float *output, int len, int maskWidth){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    float value = 0.0;
    int convolutionStartIndex = index - maskWidth/2;
    if (index < len){
       
        for (int i = 0; i < maskWidth; i++){
            if (convolutionStartIndex + i >= 0 && convolutionStartIndex + i < len) {
                value += input[convolutionStartIndex + i] * d_mask[i];
            }
        }
        output[index] = value;
    }
}

/*
    In this kernel, we will implement 2D convolution with CUDA
    grid = (col / output_tile_col, row / output_tile_row) 
    block = (output_tile_col + mask_width / 2, output_tile_col + mask / 2)
    shared memory = block

*/ 

__global__
void convolution2D(float *input, float *output, int row, int col, int patch, int maskWidth){
    __shared__ float inputTile[O_TILE_SIZE + MASK_WIDTH - 1][O_TILE_SIZE + MASK_WIDTH - 1];
    int tx = threadIdx.x; int ty = threadIdx.y;
    int outputCol = blockIdx.x * O_TILE_SIZE + tx;
    int outputRow = blockIdx.y * O_TILE_SIZE + ty;
    int inputCol = outputCol - MASK_WIDTH / 2;
    int inputRow = outputRow - MASK_WIDTH / 2;

    if (inputCol >=0 && inputCol < col && inputRow >= 0 && inputRow < row){
        inputTile[ty][tx] = input[inputRow * col + inputCol];
    }
    else {
        inputTile[ty][tx] = 0.0;
    }
    __syncthreads();
    float value = 0.0;
    if (ty < O_TILE_SIZE && tx < O_TILE_SIZE){
        for (int i = 0; i < MASK_WIDTH; i++){
            for (int j = 0 ; j < MASK_WIDTH; j++){
               value += inputTile[i + ty][j + tx] * d_mask2D[i][j];
            }
        }
        if (outputCol < col && outputRow < row){
            output[outputRow * col + outputCol] = value;
        }
    }
}