#include<string>
#include<stdio.h>
#include<stdlib.h>



template <class T>
void peakMatrix (T* matrix, int maxRow, int maxCol, bool singleValue = false, int row = 0, int col = 0)
{
    if (singleValue){
        std :: cout << "matrix["<< row <<"][" << col <<"] :"<< matrix[row * maxRow + col] << std :: endl;
        
    }
    else {
        for (int i = 0; i < maxRow; i++){
            for (int j= 0; j < maxRow; j++){
            std :: cout << "\t" << matrix[i * maxRow + j];   
        }
        std :: cout << "\n";
        }
    }
}

template <class T>
bool checkAdditionResult (T* first, T* second, T* result, int row, int col)
{
    int index = 0;
    for (int i = 0; i < row; i++){
        for (int j =0; j < col; j++){
            if (result[index] != first[index] + second[index]){
                printf("matrix addition not correct at index %d:\n", index);
                std :: cout << "\tgiven result :" << result[index];
                std :: cout << "\t \t calculate result: " << first[index] + second[index];
                std :: cout << endl;
                return false;
            }
            index++;
        }
    }
    printf("matrix addition correct. \n");
    return true;
}

template <class T>
bool checkMatrixVectorMult(T* matrix, T* vector, T* result, int row, int col){
    for (int i =0 ; i < row; i++){
        float tmp = 0.0;
        for (int j = 0; j < col; j++){
            tmp += matrix[i * row + j] * vector[i];
        }
        if (result[i] != tmp){
            printf("matrix vector mult not correct at index %d:\n", i);
                std :: cout << "\tgiven result :" << result[i];
                std :: cout << "\t \t calculate result: " << tmp;
                std :: cout << endl;
                return false;
        }
    }
    printf("matrix vector mult correct. \n");
    return true;
}

/*
    colLeft = rowRight
*/
template <class T>
bool checkMatrixMatrixMult(T* matrixLeft, T* matrixRight, T* result, 
                            int rowLeft, int colLeft,
                            int rowRight, int colRight){
    if (colLeft != rowRight){
        printf("Invalid Inputs. \n");
        return false;
    }
    for (int i = 0; i < rowLeft; i++){
        for (int j = 0; j < colRight; j++){
            //calculate one element of the result matrix
            float value = 0.0;
            for (int k =0; k < colRight; k++){
                value += matrixLeft[i * rowLeft + k] * matrixRight[k * rowRight + j];
            }
            if (result[i * rowLeft + j] - value > 0.001){
                printf("matrix matrix mult not correct at  [%d, %d]:\n", i, j);
                std :: cout << "\tgiven result :" << result[i * rowLeft + j];
                std :: cout << "\t \t calculate result: " << value;
                std :: cout << "\t difference : " << result[i * rowLeft + j] - value;
                std :: cout << endl;
                return false;   
            }
        }
    }
    printf("matrix matrix mult correct. \n");
    return true;
}






