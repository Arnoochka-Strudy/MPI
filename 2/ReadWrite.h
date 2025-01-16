#include <stdio.h>
#include "../Vector/vector.h"

typedef struct Pair
{
    int size;
    vector_t first;
    vector_t second;
} pair_t;


pair_t read_file(char* filename, int mpi_comm_size) {
    FILE* file;
    pair_t vecs;
    file = fopen(filename, "r");

    if (file == NULL) {
        printf("Не удалось открыть файл %s\n", filename);
        return vecs;
    }

    int num, size, size_old;

    if (fscanf(file, "%d %d", &num, &size) != 2) {
        printf("Ошибка при чтении чисел\n");
        fclose(file);
        return vecs;
    }
    size_old = size;
    size += mpi_comm_size - (size % mpi_comm_size);

    vecs.size = size;

    vecs.first = init_vector(size);
    read_vector(&vecs.first, file);

    vecs.second = init_vector(size);
    read_vector(&vecs.second, file);

    for (int i = size_old; i < size; ++i) {
        vecs.first.array[i] = 0;
        vecs.second.array[i] = 0;
    }

    fclose(file);

    return vecs;
}

void write_file(char* filename, int result) {
    FILE* file;
    file = fopen(filename, "w");
    fprintf(file, "%d\n", result);
}