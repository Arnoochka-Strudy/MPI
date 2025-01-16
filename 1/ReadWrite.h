#include <stdio.h>
#include "../Vector/vector.h"



vector_t read_file(char* filename) {
    FILE* file;
    vector_t vec;
    file = fopen(filename, "r");

    if (file == NULL) {
        printf("Не удалось открыть файл %s\n", filename);
        return vec;
    }

    int num, size;

    if (fscanf(file, "%d %d", &num, &size) != 2) {
        printf("Ошибка при чтении чисел\n");
        fclose(file);
        return vec;
    }
    vec = init_vector(size);
    read_vector(&vec, file);

    fclose(file);

    return vec;
}

void write_file(char* filename, int result_max, int result_min) {
    FILE* file;
    file = fopen(filename, "w");
    fprintf(file, "%d %d\n", result_max, result_min);
}