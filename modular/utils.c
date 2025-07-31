#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>     // shm_open, O_CREAT, O_RDWR
#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    // ftruncate, close
#include <errno.h>
#include "utils.h"

KeyValue default_dict[] = {
    {MATRIX_A_NAME, ADDR_MATRIX_A},
    {MATRIX_B_NAME, ADDR_MATRIX_B},
    {MATRIX_C_NAME, ADDR_MATRIX_C},
};

// Generate random double between fMin and fMax
double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

// Write matrix to file
void write_matrix_to_bin(const char *filename, double *matrix, int rows, int cols,long offset) {
    FILE *fp = fopen(filename, "r+b");
    if (!fp) {
        // Create the file if it doesn't exist
        fp = fopen(filename, "wb+");
        if (!fp) {
        perror("File open failed");
        exit(1);}
    }
    fseek(fp, offset, SEEK_SET);
    fwrite(matrix, sizeof(double), rows * cols, fp);

    /*for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%.6f ", matrix[i * cols + j]);
        }
        fprintf(fp, "\n");
    }*/
    
    

    fclose(fp);
}

void convert_bin_to_txt(const char *bin_file, const char *txt_file, int rows, int cols) {
    FILE *bin_fp = fopen(bin_file, "r+b");
    if (!bin_fp) {
        perror("Failed to open binary file");
        exit(EXIT_FAILURE);
    }

    FILE *txt_fp = fopen(txt_file, "w");
    if (!txt_fp) {
        perror("Failed to open text file");
        fclose(bin_fp);
        exit(EXIT_FAILURE);
    }

    double value;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fread(&value, sizeof(double), 1, bin_fp) != 1) {
                fprintf(stderr, "Error reading value at [%d][%d]\n", i, j);
                fclose(bin_fp);
                fclose(txt_fp);
                exit(EXIT_FAILURE);
            }
            fprintf(txt_fp, "%.6f ", value);
        }
        fprintf(txt_fp, "\n");
    }

    fclose(bin_fp);
    fclose(txt_fp);
}

// Write matrix to file
void write_matrix_to_file(const char *filename, double *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }
    //printf("matrix[%d] = %.2f\n", 1, matrix[1]);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%.6f ", matrix[i * cols + j]);
            //printf("matrix[%d] = %.2f\n", i * cols + j, matrix[i * cols + j]);

        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}


int make_task(int argc, char **argv) {
     if (argc < 1) {
        fprintf(stderr, "Usage: <task_name> [-flag value ...]\n");
        return 1;
    }

    const char *task_name = argv[0];
    char make_cmd[512];
    snprintf(make_cmd, sizeof(make_cmd), "make %s", task_name);

    for (int i = 1; i < argc; i += 2) {
        if (argv[i][0] != '-') {
            fprintf(stderr, "Expected flag starting with '-', got '%s'\n", argv[i]);
            return 1;
        }

        if (i + 1 >= argc) {
            fprintf(stderr, "Missing value for flag: %s\n", argv[i]);
            return 1;
        }

        const char *flag = argv[i] + 1;  // remove '-'
        const char *value = argv[i + 1];

        char param[128];
        snprintf(param, sizeof(param), " %s=%s", flag, value);
        strncat(make_cmd, param, sizeof(make_cmd) - strlen(make_cmd) - 1);
    }

    printf("[BUILD] Running: %s\n", make_cmd);
    int ret = system(make_cmd);

    if (ret != 0) {
        fprintf(stderr, "[ERROR] Build failed for task '%s'\n", task_name);
        return 1;
    }

    return 0;
}

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

     /* For Virtual memory on gem5
     void* ptr = mmap(fixed_addr, matrix_size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_FIXED, fd, 0);*/
                     
    //For Host Testing
    void* ptr = mmap(NULL, matrix_size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS , fd, 0);

    
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

