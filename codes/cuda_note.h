#include<cuda.h>
#include<string>
#include<stdio.h>
#include"cn_data_structure.h"


using namespace std;



void platform_info();

/*
  print out current platform infomation
  return device pointer
  store device grid limitation and block limitation
*/
void platform_info(int *dev_count, bool dev_info =true){
  cudaError_t err;
  //int a = 0;
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
        printf("\t Peak Memory Bandwidth (GB/s): %f\n\n",2.0*dev_prop.memoryClockRate*(dev_prop.memoryBusWidth/8)/1.0e6);
        dev_limit *new_dev = new dev_limit(dev_prop.maxGridSize, dev_prop.maxThreadsDim);
        dev.push_back(new_dev);
      }
    }
  }
}
