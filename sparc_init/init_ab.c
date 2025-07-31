#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<string.h>
#include <fcntl.h>     // shm_open, O_CREAT, O_RDWR
#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    // ftruncate, close
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


#define ADDR_MATRIX_A ((void*)0x05000000)
#define ADDR_MATRIX_B ((void*)0x06000000)
#define ADDR_MATRIX_C ((void*)0x07000000)
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
        perror("Invalid matrix name");
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
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    /*if (fd < 0) {
        perror("File open failed");
        exit(1);
    }*/

    char buffer[128];
    for (int i = 0; i < rows; i++) {
        int len = 0;
        for (int j = 0; j < cols; j++) {
            len += snprintf(buffer + len, sizeof(buffer) - len, "%.6f ", matrix[i * cols + j]);
            // Flush buffer if full
            if (len > sizeof(buffer) - 32) {
                write(fd, buffer, len);
                len = 0;
            }
        }
        buffer[len++] = '\n';
        write(fd, buffer, len);
    }

    close(fd);
}

int attach_to_fd(){
	int fd = open("my_output.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd < 0) {
        //perror("open");
        return -1;
        }
    return fd;    
}


int main(int argc, char **argv) {
    /*int out_fd = open("my_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("open failed");
        exit(1);
    }
    dup2(out_fd, STDOUT_FILENO);
    close(out_fd);*/
    int fd = attach_to_fd();
    
    if (argc < 2) {
        //dprintf(fd,"Usage: %s <matrix_dimension>\n", argv[0]);
        return 1;
    }

    int matrix_dim = atoi(argv[1]);
    if (matrix_dim <= 0) {
        //dprintf(fd,"Matrix dimension must be a positive integer.\n");
        return 1;
    }

    srand(getpid());  // Seed for random number generation

    // Allocate matrices
    //double* value = (double*)ADDR_FD;
    //dprintf(fd,"Memory allocation Starting....:\n");
    //close(fd);
    /*double* value = (double*)mmap(NULL, sizeof(double),
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED| MAP_ANONYMOUS, -1, 0);
    /*if (value == MAP_FAILED) {
        perror("mmap");
        return 1;
    }*/
    //dprintf(fd,"Memory FD....%f\n:",*value);

    // Initialize with random values
    #ifdef INIT_A
    //*value = 420.0;
    //dprintf(fd,"Memory FD....%f\n:",*value);
    double* A = setup_shared_matrix(MATRIX_A_NAME, matrix_dim);
    if (!A) {
        dprintf(fd,"Memory allocation of matrix A failed.\n");
        return 1;
    }
    initialize_matrix(A, matrix_dim, matrix_dim);
    write_matrix_to_file("core0_out.txt", A, matrix_dim, matrix_dim);
    dprintf(fd,"Matrix A initialised with start address: %p and first element:%f\n",A,A[0]);
    double* B = setup_shared_matrix(MATRIX_B_NAME, matrix_dim);
    dprintf(fd,"Matrix B accessed with start address: %p and first element:%f\n",B,B[0]);
    //free(A);
    //cleanup_shared_matrix(MATRIX_A_NAME, matrix_dim);

    #endif
    
    #ifdef INIT_B
    //double *B = (double *)malloc(matrix_dim * matrix_dim * sizeof(double));
    //dprintf(fd,"Memory FD....%f\n:",value);
    double* B = setup_shared_matrix(MATRIX_B_NAME, matrix_dim);
    if (!B) {
        dprintf(fd,"Memory allocation of matrix B failed.\n");
        return 1;
    }
    initialize_matrix(B, matrix_dim, matrix_dim);
    write_matrix_to_file("core1_out.txt", B, matrix_dim, matrix_dim);
    dprintf(fd,"Matrix B initialised with start address: %p and first element:%f\n",B,B[0]);
    double* A = setup_shared_matrix(MATRIX_A_NAME, matrix_dim);
    dprintf(fd,"Matrix A accessed with start address: %p and first element:%f\n",A,A[0]);
    //free(B);
    //cleanup_shared_matrix(MATRIX_B_NAME, matrix_dim);
    #endif

    
    cleanup_shared_matrix(MATRIX_A_NAME, matrix_dim);
    cleanup_shared_matrix(MATRIX_B_NAME, matrix_dim);
    A = NULL;
    B = NULL;
    

    close(fd);
    
    return 0;
}

