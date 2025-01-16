#include <stdio.h>
#include "../Vector/vector.h"
#include "ReadWriteStriped.h"
#include <time.h>

int matmul(int* row, int* column, int size) {
    int result = 0;

    for (int idx = 0; idx < size; ++idx) {
        result += row[idx] * column[idx];
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        return 1;
    }
    clock_t start = clock();

    pair_t matrixs = read_file(argv[1], argv[2], 1);
    int size = matrixs.first.m;

    matrix_t result = init_matrix(matrixs.first.n, matrixs.second.n);

    for (int i = 0; i < matrixs.first.n; ++i) {
        for (int j = 0; j < matrixs.second.n; ++j) {
            result.array[i][j] = matmul(matrixs.first.array[i], matrixs.second.array[j], size);
        }
    }

    write_file(argv[3], result);

    free(matrixs.first.array);
    free(matrixs.second.array);
    free(result.array);

    clock_t end = clock();

    printf("%f\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}