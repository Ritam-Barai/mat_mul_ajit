#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<string.h>
#include <fcntl.h>     // shm_open, O_CREAT, O_RDWR
#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    // ftruncate, close
#include <cerrno>
#include <errno.h>


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

// Define a struct for key-value pairs
typedef struct {
    const char* key;
     void* value;
} KeyValue;

#define ADDR_MATRIX_A ((void*)0x04000000)
#define ADDR_MATRIX_B ((void*)0x05000000)
#define ADDR_MATRIX_C ((void*)0x06000000)
#define MATRIX_A_NAME "/matrix_A"
#define MATRIX_B_NAME "/matrix_B"
#define MATRIX_C_NAME "/matrix_C"

// Define the "dictionary"
KeyValue default_dict[] = {
    {MATRIX_A_NAME, ADDR_MATRIX_A},
    {MATRIX_B_NAME, ADDR_MATRIX_B},
    {MATRIX_C_NAME, ADDR_MATRIX_C},
};

// Total number of key-value pairs
#define NUM_KEYS (sizeof(default_dict)/sizeof(default_dict[0]))

// Function to get value by key
void* get_address_from_key(const char* input_key) {
    for (int i = 0; i < NUM_KEYS; i++) {
        if (strcmp(default_dict[i].key, input_key) == 0) {
            return default_dict[i].value;
        }
    }
    return NULL;
}

double* setup_shared_matrix(const char* shm_name, int matrix_dim) {
    size_t matrix_size = matrix_dim * matrix_dim * sizeof(double);
     void* fixed_addr = get_address_from_key(shm_name);
    if (!fixed_addr) {
        fprintf(stderr, "Invalid matrix name: %s\n", shm_name);
        return NULL;
    }

    int fd = shm_open(shm_name, O_RDWR, 0666);
    if (fd == -1 && errno == ENOENT) {
        // Not found, we will create
        
        fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
        if (fd == -1) {
            perror("shm_open create failed");
            return NULL;
        }
        if (ftruncate(fd, matrix_size) == -1) {
            perror("ftruncate failed");
            return NULL;
        }
    } else if (fd == -1) {
        perror("shm_open failed");
        return NULL;
    } 

     void* ptr = mmap(fixed_addr, matrix_size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_FIXED, fd, 0);

    close(fd);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        return NULL;
    }

    return (double*)ptr;
}

void cleanup_shared_matrix(const char* shm_name, int matrix_dim) {
    size_t matrix_size = matrix_dim * matrix_dim * sizeof(double);
     void* fixed_addr = get_address_from_key(shm_name);
    if(fixed_addr && matrix_size > 0)
        munmap(fixed_addr , matrix_size );
        shm_unlink(shm_name);
    
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
    //double *B = (double *)malloc(matrix_dim * matrix_dim * sizeof(double));
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

