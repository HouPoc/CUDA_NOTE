#include<iostream>
#include"cuda_note.h"

int main(){
  int dev_count = 0;
  platform_info(&dev_count);
  return 0;
}
