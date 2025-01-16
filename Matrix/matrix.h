# pragma once

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct matrix
{
    int n, m;
    int** array;
} matrix_t;

matrix_t init_matrix(int n, int m) {
    matrix_t matrix;
    matrix.n = n;
    matrix.m = m;

    matrix.array = malloc(sizeof(int*) * n);

    for (int i = 0; i < n; ++i){
        matrix.array[i] = malloc(sizeof(int) * m);
        for (int j = 0; j < m; ++j){
            matrix.array[i][j] = 0;
        }
    }
    return matrix;
}

void read_matrix(matrix_t* matrix, FILE* file) {
    for (int row = 0; row < matrix->n; ++row) {
        for (int column = 0; column < matrix->m; ++column){
            fscanf(file, "%d", &matrix->array[row][column]);
        } 
    }
}

void read_matrix_T(matrix_t* matrix, FILE* file) {
    for (int row = 0; row < matrix->m; ++row) {
        for (int column = 0; column < matrix->n; ++column){
            fscanf(file, "%d", &matrix->array[column][row]);
        } 
    }
}

void write_matrix(matrix_t* matrix, FILE* file) {
    fprintf(file, "%d %d\n", matrix->n, matrix->m);
    for (int i = 0; i < matrix->n; ++i) {
        for (int j = 0; j < matrix->m - 1; ++j) {
            fprintf(file, "%d ", matrix->array[i][j]);
        }
        fprintf(file, "%d\n", matrix->array[i][matrix->m - 1]);
    }
}

void print_matrix(matrix_t* matrix) {
    printf("size: %d (n), %d (m)\n", matrix->n, matrix->m);
    for (int i = 0; i < matrix->n; ++i){
        for(int j = 0; j < matrix->m - 1; ++j) {
            printf("%d ", matrix->array[i][j]);
        }
        printf("%d\n", matrix->array[i][matrix->m - 1]);
    }
}
