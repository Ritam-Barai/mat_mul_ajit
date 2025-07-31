#ifndef UTILS_H
#define UTILS_H

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
#define DEFAULT_MATRIX_DIM 32

// Define the "dictionary"
extern KeyValue default_dict[]; 

// Total number of key-value pairs
#define NUM_KEYS (sizeof(default_dict)/sizeof(default_dict[0]))


double fRand(double fMin, double fMax);
void write_matrix_to_file(const char *filename, double *matrix, int rows, int cols);
void write_matrix_to_bin(const char *filename, double *matrix, int rows, int cols,long offset);
void convert_bin_to_txt(const char *bin_file, const char *txt_file, int rows, int cols);
int make_task(int argc, char **argv);
void* get_address_from_key(const char* input_key);
double* setup_shared_matrix(const char* shm_name, int matrix_dim);
void cleanup_shared_matrix(const char* shm_name, int matrix_dim);
#endif
