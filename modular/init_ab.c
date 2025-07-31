#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "init_ab.h"
#include "utils.h"

// Initialize matrix with random values
void initialize_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
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
    printf("%d",matrix_dim);
    if (matrix_dim <= 0) {
        printf("Matrix dimension must be a positive integer.\n");
        return 1;
    }

    srand(time(NULL));  // Seed for random number generation

    // Allocate matrices
    
    
    

    // Initialize with random values
    #ifdef INIT_A
    double* A = setup_shared_matrix(MATRIX_A_NAME, matrix_dim);
    if (!A) {
        printf("Memory allocation of matrix A failed.\n");
        return 1;
    }
    initialize_matrix(A, matrix_dim, matrix_dim);
    write_matrix_to_file("core0_out.txt", A, matrix_dim, matrix_dim);
    printf("Matrix A initialised with start address: %p\n",A);
    double* B = setup_shared_matrix(MATRIX_B_NAME, matrix_dim);
    //free(A);
    cleanup_shared_matrix(MATRIX_A_NAME, matrix_dim);
    A = NULL;
    B = NULL;
    #endif
    
    #ifdef INIT_B
    double* B = setup_shared_matrix(MATRIX_B_NAME, matrix_dim);
    if (!B) {
        printf("Memory allocation of matrix B failed.\n");
        return 1;
    }
    initialize_matrix(B, matrix_dim, matrix_dim);
    write_matrix_to_file("core1_out.txt", B, matrix_dim, matrix_dim);
    printf("Matrix B initialised with start address: %p\n",B);
    double* A = setup_shared_matrix(MATRIX_A_NAME, matrix_dim);
    //free(B);
    cleanup_shared_matrix(MATRIX_B_NAME, matrix_dim);
    A = NULL;
    B = NULL;
    #endif

    
    
    

    
    
    return 0;
}

