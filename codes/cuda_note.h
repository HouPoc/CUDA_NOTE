#include<cuda.h>
#include<string>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include"cn_data_structure.h"


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
            matrix[i * row + j] = (T) (random? 100.0 *((float) rand()) / (float) RAND_MAX : 0);
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

