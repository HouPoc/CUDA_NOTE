#include<iostream>
#include"../cuda_note.h"

int main(){
  int dev_count = 0;
  cudaError_t err = cudaGetDeviceCount(&dev_count);
  if (err == cudaSuccess)
  {
    platformInfo(&dev_count);
  }
  return 0;
}
