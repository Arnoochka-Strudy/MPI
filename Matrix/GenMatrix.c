#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <time.h>

// генерация матрицы, необходимые значения: n (rows), m (columns), max, filename

int main(int argc, char* argv[]) {
    srand(time(0));
    if (argc < 5) {
        return 1;
    }
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int max = atoi(argv[3]);
    char* filename = argv[4];

    FILE* file = fopen(filename, "w");
    matrix_t matrix = init_matrix(n, m);

    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            matrix.array[i][j] = rand() % max;
            if (rand() % 2) matrix.array[i][j] = -matrix.array[i][j];
        }
    }
    write_matrix(&matrix, file);
    fclose(file);

    free(matrix.array);

    return 0;
}