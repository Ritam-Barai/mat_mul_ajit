#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "init_c.h"
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>




// Initialize matrix with random values
void initialize_result_matrix(double *matrix, int rows_start , int rows_end , int cols) {
    
    for (int i = rows_start; i < rows_end; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i * cols + j] = fRand(0.0001, 1.0000);
        }
    }
}



int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <matrix_dimension>\n", argv[0]);
        return 1;
    }

    int matrix_dim = atoi(argv[1]) ;
    if (matrix_dim <= 0) {
        printf("Matrix dimension must be a positive integer.\n");
        return 1;
    }

     // Allocate matrices
    double* C = setup_shared_matrix(MATRIX_C_NAME, matrix_dim);
    if (!C) {
        printf("Memory allocation of matrix C failed.\n");
        return 1;
    }
    
    

    // Initialize with random values
    #ifdef INIT_C_UP
    /*// Allocate matrices
    double* C = setup_shared_matrix(MATRIX_C_NAME, matrix_dim);
    if (!C) {
        printf("Memory allocation of matrix C failed.\n");
        return 1;
    }*/
    initialize_result_matrix(C,0, matrix_dim/2, matrix_dim);
    write_matrix_to_bin("core_rst.bin", C, matrix_dim/2, matrix_dim,0);
    printf("Matrix C initialised with start address: %p\n",C);
    //cleanup_shared_matrix(MATRIX_C_NAME, matrix_dim);
    //C = NULL;
    
    #endif
    
    #ifdef INIT_C_DOWN
   
    initialize_result_matrix(C,matrix_dim/2 , matrix_dim, matrix_dim);
    write_matrix_to_bin("core_rst.bin", C+matrix_dim/2 * matrix_dim , matrix_dim - matrix_dim/2, matrix_dim,matrix_dim/2 * (matrix_dim * sizeof(double)));
    printf("Matrix C initialised with start address: %p\n",C);
    
    #endif
    
    convert_bin_to_txt("core_rst.bin", "core_rst.txt", matrix_dim, matrix_dim);
    cleanup_shared_matrix(MATRIX_C_NAME, matrix_dim);
    C = NULL;
    

    
    
    return 0;
}

