#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>     // shm_open, O_CREAT, O_RDWR
#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    // ftruncate, close
#include <errno.h>

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

void cleanup_shared_matrix(const char* shm_name, int matrix_dim) {
    size_t matrix_size = matrix_dim * matrix_dim * sizeof(double);
    void* fixed_addr = get_address_from_key(shm_name);
    if (fixed_addr && matrix_size > 0) {
        munmap(fixed_addr, matrix_size);
        shm_unlink(shm_name);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <matrix_dimension>\n", argv[0]);
        return 1;
    }

    int matrix_dim = atoi(argv[1]);
    if (matrix_dim <= 0) {
        fprintf(stderr, "Matrix dimension must be a positive integer.\n");
        return 1;
    }

    cleanup_shared_matrix(MATRIX_A_NAME, matrix_dim);
    cleanup_shared_matrix(MATRIX_B_NAME, matrix_dim);

    return 0;
}

