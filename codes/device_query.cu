#include<iostream>
#include"cuda_note.h"

int main(){
  int *dev_count = NULL;
  platform_info(dev_count);
  return 0;
}
