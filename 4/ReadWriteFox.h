#include <stdio.h>
#include "../Matrix/matrix.h"

#define max(x, y) ( (x) > (y) ? (x) : (y) )
#define min(x, y) ( (x) < (y) ? (x) : (y) )

int n_old;

typedef struct Pair
{
    matrix_t first;
    matrix_t second;
} pair_t;

pair_t read_file(char* filename_A, char* filename_B, int mpi_comm_size) {
    FILE* file_A;
    FILE* file_B;
    pair_t matrixs;
    file_A = fopen(filename_A, "r");
    file_B = fopen(filename_B, "r");

    if (file_A == NULL || file_B == NULL) {
        printf("Не удалось открыть файл один из файлов\n");
        return matrixs;
    }

    int n_A, m_A, n_B, m_B;

    if (fscanf(file_A, "%d %d", &n_A, &m_A) != 2) {
        printf("Ошибка при чтении чисел из файла %s\n", filename_A);
        fclose(file_A);
        return matrixs;
    }

    if (fscanf(file_B, "%d %d", &m_B, &n_B) != 2) {
        printf("Ошибка при чтении чисел из файла %s\n", filename_B);
        fclose(file_B);
        return matrixs;
    }

    n_old = n_A;

    if (n_A % mpi_comm_size) {
        n_A += mpi_comm_size - (n_A % mpi_comm_size);
    }
    
    int n = n_A;


    matrixs.first = init_matrix(n, n);
    matrixs.second = init_matrix(n, n);

    read_matrix(&matrixs.first, file_A);
    read_matrix(&matrixs.second, file_B);

    fclose(file_A);
    fclose(file_B);

    return matrixs;
}

void write_file(char* filename, matrix_t result) {
    FILE* file;
    file = fopen(filename, "w");
    result.n = n_old;
    write_matrix(&result, file);
}