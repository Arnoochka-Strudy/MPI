#include <stdio.h>
#include "../Vector/vector.h"
#include "ReadWrite.h"
#include <time.h>



int main(int argc, char* argv[]) {
    if (argc < 3) {
        return 1;
    }
    clock_t start = clock();
    vector_t vec = read_file(argv[1]);
    
    int result_max = vec.array[0], result_min = vec.array[0], el;
    for (int i = 0; i < vec.size; ++i) {
        el = vec.array[i];
        if (result_max < el) result_max = el;
        if (result_min > el) result_min = el;
    }

    write_file(argv[2], result_max, result_min);
    clock_t end = clock();

    free(vec.array);
    
    printf("%f\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}