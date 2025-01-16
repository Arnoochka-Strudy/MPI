#include <stdio.h>
#include "../Vector/vector.h"
#include "ReadWrite.h"
#include <time.h>



int main(int argc, char* argv[]) {
    if (argc < 3) {
        return 1;
    }
    clock_t start = clock();

    pair_t vecs = read_file(argv[1], 1);
    
    long long result = 0;
    for (int i = 0; i < vecs.size; ++i) {
        result += vecs.first.array[i] * vecs.second.array[i];
    }
    
    write_file(argv[2], result);

    clock_t end = clock();

    free(vecs.first.array);
    free(vecs.second.array);

    printf("%f\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}