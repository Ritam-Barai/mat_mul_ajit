#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Generate random double between fMin and fMax
double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

// Initialize matrix with random values
void initialize_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i * cols + j] = fRand(0.0001, 1.0000);
        }
    }
}

// Write matrix to file
void write_matrix_to_file(const char *filename, double *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%.6f ", matrix[i * cols + j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <matrix_dimension>\n", argv[0]);
        return 1;
    }

    int matrix_dim = atoi(argv[1]);
    if (matrix_dim <= 0) {
        printf("Matrix dimension must be a positive integer.\n");
        return 1;
    }

    srand(time(NULL));  // Seed for random number generation

    // Allocate matrices
    
    
    

    // Initialize with random values
    #ifdef INIT_A
    double *A = (double *)malloc(matrix_dim * matrix_dim * sizeof(double));
    if (!A) {
        printf("Memory allocation of matrix A failed.\n");
        return 1;
    }
    initialize_matrix(A, matrix_dim, matrix_dim);
    write_matrix_to_file("core0_out.txt", A, matrix_dim, matrix_dim);
    printf("Matrix A initialised\n");
    free(A);
    #endif
    
    #ifdef INIT_B
    double *B = (double *)malloc(matrix_dim * matrix_dim * sizeof(double));
    if (!B) {
        printf("Memory allocation of matrix B failed.\n");
        return 1;
    }
    initialize_matrix(B, matrix_dim, matrix_dim);
    write_matrix_to_file("core1_out.txt", B, matrix_dim, matrix_dim);
    printf("Matrix B initialised\n");
    free(B);
    #endif

    
    
    

    
    
    return 0;
}

