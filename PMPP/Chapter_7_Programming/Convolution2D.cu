#include "../cuda_debug.h"
#include "../cuda_note.h"

#define DATA_DIM 1000

int main() {
    int h_row = DATA_DIM;
    int h_col = DATA_DIM;
    size_t maskSize = sizeof(float) * MASK_WIDTH * MASK_WIDTH;
    size_t dataSize = sizeof(float) * DATA_DIM * DATA_DIM;
    float *h_mask2D = (float*)malloc(maskSize);
    float *h_input = (float*) malloc(dataSize);
    float *h_output = (float*) malloc(dataSize);
    initial2DMatrix<float>(h_input, DATA_DIM, DATA_DIM, 1);
    initial2DMatrix<float>(h_mask2D, MASK_WIDTH, MASK_WIDTH, 1);
    float *d_input, *d_output;

    cudaMalloc(&d_input, dataSize);
    cudaMalloc(&d_output, dataSize);

    cudaMemcpy(d_input, h_input, dataSize, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(d_mask2D, h_mask2D, maskSize);
 
    dim3 block(I_TILE_SIZE, I_TILE_SIZE, 1);

    dim3 grid(ceil(DATA_DIM/(float)O_TILE_SIZE), ceil(DATA_DIM/(float)O_TILE_SIZE), 1);
    
    convolution2D<<<grid, block>>>(d_input, d_output, h_row, h_col, (h_col + MASK_WIDTH - 1), MASK_WIDTH);
    
    std :: cout << cudaGetErrorString(cudaGetLastError()) << std :: endl;
    cudaMemcpy(h_output, d_output, dataSize, cudaMemcpyDeviceToHost);
    cudaFree(d_input);
    cudaFree(d_output);
    check2Dconvolution<float>(h_input, h_mask2D, h_output, h_row, h_col, MASK_WIDTH, MASK_WIDTH);
    free(h_input);
    free(h_output);
    free(h_mask2D);

    return 0;
}