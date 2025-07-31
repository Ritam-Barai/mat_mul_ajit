#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"
#include "init_ab.h"

int main(int argc, char **argv) {

    for (int i = 0; i < argc; i += 1) {
    	fprintf(stderr, "Expected flags are '%s'\n", argv[i]);
    }
    /*double *test = (double *)malloc(1024 * 1024 * sizeof(double));
    printf("Address:%p",test);
    free(test);*/
    
    char *args[] = {"init_Cup", "-MATRIX_DIM", "64"};
    make_task(3, args);
    
    args[0] = "init_Cdown";
    make_task(3, args);
    
    return 0;
}

