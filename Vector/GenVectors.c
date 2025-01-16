#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include <time.h>

// 1 - генерация 1-го вектора, необходимо задать size, max, filename
// 2 - генерация пары векторов, необходимо задать size, max, filename

void gen_vectors(int size, int max, char* filename) {
    FILE* file = fopen(filename, "w");
    fprintf(file, "%d %d\n", max, size);

    vector_t vec = init_vector(size);
    vec.size = size;

    #pragma omp parallel for
    for (int idx = 0; idx < size; ++idx) {
        vec.array[idx] = rand() % max;
        if (rand() % 2) vec.array[idx] = -vec.array[idx];
    }

    write_vector(&vec, file);

    free(vec.array);
    fclose(file);
}



void gen_vector_pair(int size, int max, char* filename) {
    FILE* file = fopen(filename, "w");
    fprintf(file, "%d %d\n", max, size);

    vector_t vec1 = init_vector(size);
    vector_t vec2 = init_vector(size);
    vec1.size = size;
    vec2.size = size;

    #pragma omp parallel for
    for (int idx = 0; idx < size; ++idx) {
        vec1.array[idx] = rand() % max;
        vec2.array[idx] = rand() % max;
        if (rand() % 2) vec1.array[idx] = -vec1.array[idx];
        if (rand() % 2) vec2.array[idx] = -vec2.array[idx];
    }

    write_vector(&vec1, file);
    write_vector(&vec2, file);

    free(vec1.array);
    free(vec2.array);
    fclose(file);
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if (argc <= 1) {
        return 1;
    }
    switch (atoi(argv[1]))
    {
    case 1: {
        int size = atoi(argv[2]);
        int max = atoi(argv[3]);
        char* filename = argv[4];
        gen_vectors(size, max, filename);
        break;
    }
    case 2: {
        int size = atoi(argv[2]);
        int max = atoi(argv[3]);
        char* filename = argv[4];
        gen_vector_pair(size, max, filename);
        break;
    }
    default: {
        printf("uncnown command: %s", argv[1]);
        break;
    }
    }

    return 0;
}