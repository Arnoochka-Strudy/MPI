#include <stdio.h>
#include "ReadWriteStriped.h"
#include <time.h>
#include <mpi.h>

int scalar_mul(int* row, int* column, int size) {
    int result = 0;

    for (int idx = 0; idx < size; ++idx) {
        result += row[idx] * column[idx];
    }

    return result;
}

void send_recv_matrix(matrix_t* send_matrix, matrix_t* recv_matrix, int isRoot, int ProcNum, int* size) {
    if (isRoot){
        for (int proc_id = 1; proc_id < ProcNum; ++proc_id) {
            for (int idx = 0; idx < size[0]; ++idx) {
                MPI_Send(send_matrix->array[idx + proc_id*size[0]], size[2], MPI_INT, proc_id, idx, MPI_COMM_WORLD);
            }
        }
    } else {
        for (int idx = 0; idx < size[0]; ++idx) {
            MPI_Recv(recv_matrix->array[idx], size[2], MPI_INT, 0, idx, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        return 1;
    }

    int ProcNum, ProcRank, RecvRank;
    MPI_Init(&argc, &argv);
    double start = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int isRoot = ProcRank == 0;

    pair_t matrixs;
    matrix_t result;
    int size[3];

    if (isRoot){
        matrixs = read_file(argv[1], argv[2], ProcNum);
        result = init_matrix(matrixs.first.n, matrixs.second.n);
        size[0] = matrixs.first.n / ProcNum;
        size[1] = matrixs.second.n;
        size[2] = matrixs.first.m;
    }

    MPI_Bcast(size, 3, MPI_INT, 0, MPI_COMM_WORLD);

    if (!isRoot){
        result = init_matrix(size[0], size[1]);
        matrixs.first = init_matrix(size[0], size[2]);
        matrixs.second = init_matrix(size[0], size[2]);
    }

    if (isRoot) {
        send_recv_matrix(&matrixs.first, NULL, 1, ProcNum, size);
    } else {
        send_recv_matrix(NULL, &matrixs.first, 0, ProcNum, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (isRoot) {
        send_recv_matrix(&matrixs.second, NULL, 1, ProcNum, size);
    } else {
        send_recv_matrix(NULL, &matrixs.second, 0, ProcNum, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    matrix_t send_matrix = init_matrix(size[0], size[2]), recv_matrix = init_matrix(size[0], size[2]);
    int r;
    for (int k = 0; k < ProcNum; ++k){
        for (int i = 0; i < size[0]; ++i) {
            for (int j = 0; j < size[0]; ++j) {
                if (k == 0) {
                    r = j + size[0] * ProcRank;
                } else {
                    r = j + (((ProcRank + k) % ProcNum) * size[0]);
                }
                result.array[i][r] = scalar_mul(matrixs.first.array[i], matrixs.second.array[j], size[2]);
            }
        }
        for (int idx = 0; idx < size[0]; ++idx) {
            for (int i = 0; i < size[2]; ++i) {
                send_matrix.array[idx][i] = matrixs.second.array[idx][i];
            }

            MPI_Sendrecv(send_matrix.array[idx], size[2], MPI_INT, (ProcNum + ProcRank - 1) % ProcNum, idx, \
                        recv_matrix.array[idx], size[2], MPI_INT, (ProcRank + 1) % ProcNum, idx, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int i = 0; i < size[2]; ++i) {
                matrixs.second.array[idx][i] = recv_matrix.array[idx][i];
            }
        }
    }
    free(send_matrix.array);
    free(recv_matrix.array);

    if (isRoot){
        for (int proc_id = 1; proc_id < ProcNum; ++proc_id) {
            for (int idx = 0; idx < size[0]; ++idx) {
                MPI_Recv(result.array[proc_id * size[0] + idx], size[1], MPI_INT, proc_id, idx, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    } else {
        for (int idx = 0; idx < size[0]; ++idx) {
            MPI_Send(result.array[idx], size[1], MPI_INT, 0, idx, MPI_COMM_WORLD);
        }
    }

    if (isRoot){
        write_file(argv[3], result);
    }
    free(matrixs.first.array);
    free(matrixs.second.array);
    free(result.array);

    double end = MPI_Wtime();
    MPI_Finalize();
    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;
}