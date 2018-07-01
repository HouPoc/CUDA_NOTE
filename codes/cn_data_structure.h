/*
  This file contains data structures used in this program
*/
#include<vector>

class d_3{
  public:
    int x, y, z;
    d_3(int* _para);
};

d_3 :: d_3(int* _para){
  x = _para[0];
  y = _para[1];
  z = _para[2];
}

class dev_limit{
  public:
    d_3* grid, block;
    dev_limit(int *grid_limit, int *block_limit);
};

dev_limit :: dev_limit(int *grid_limit, int *block_limit){
   grid   = new d_3(grid_limit);
   block  = new d_3(block_limit);
}

vector< dev_limit*> dev;
