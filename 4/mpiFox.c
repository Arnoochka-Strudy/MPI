#include <stdio.h>
#include "ReadWriteFox.h"
#include <time.h>
#include <mpi.h>
#include <math.h>

void matmul(int* mat1, int* mat2, int* result, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                result[j + i * size] += mat1[size * i + k] * mat2[k * size + j];
                //printf("%d %d: %d %d\n", i, j, size * i + k, k * size + j);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        return 1;
    }
    int ProcNum, ProcRank, RecvRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int mpi_comm_size = (int)sqrt(ProcNum);
    int dims[2] = {mpi_comm_size, mpi_comm_size}, periods[2] = {1, 1};
    MPI_Comm GridComm;

    double start = MPI_Wtime();

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &GridComm);

    int isRoot = ProcRank == 0;
    pair_t matrixs;
    matrix_t result;
    int size;

    if (isRoot){
        matrixs = read_file(argv[1], argv[2], mpi_comm_size);
        result = init_matrix(matrixs.first.n, matrixs.first.n);
        size = matrixs.first.n / mpi_comm_size;
    }

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int* vecA;
    int* vecB; 
    vecA = (int *)malloc(sizeof(int) * size * size);
    vecB = (int *)malloc(sizeof(int) * size * size);

    int coords[2] = {0, 0};

    if (isRoot){
        int rank = 0, idx1, idx2;
        int* vec = (int *)malloc(sizeof(int) * size * size);

        for (coords[0] = 0; coords[0] < mpi_comm_size; ++coords[0]) {
            for (coords[1] = 0; coords[1] < mpi_comm_size; ++coords[1]) {
                MPI_Cart_rank(GridComm, coords, &rank);
                if (rank == 0) continue;

                for (int i = 0; i < size; ++i) {
                    for (int j = 0; j < size; ++j) {
                        idx1 = coords[0] * size + i;
                        idx2 = ((coords[0] + coords[1]) % mpi_comm_size) * size + j;
                        vec[i * size + j] = matrixs.first.array[idx1][idx2];
                    }
                }

                MPI_Send(vec, size * size, MPI_INT, rank, 0, MPI_COMM_WORLD);

                for (int i = 0; i < size; ++i) {
                    for (int j = 0; j < size; ++j) {
                        idx1 = ((coords[0] + coords[1]) % mpi_comm_size) * size + i;
                        idx2 = coords[1] * size + j;
                        vec[i * size + j] = matrixs.second.array[idx1][idx2];
                    }
                }
                MPI_Send(vec, size * size, MPI_INT, rank, 1, MPI_COMM_WORLD);

            }
        }
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                idx1 = i;
                idx2 = j;
                vecA[i * size + j] = matrixs.first.array[idx1][idx2];
            }
        }

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                idx1 = i;
                idx2 = j;
                vecB[i * size + j] = matrixs.second.array[idx1][idx2];
            }
        }

        free(vec);
    } else {
        MPI_Recv(vecA, size * size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(vecB, size * size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Cart_coords(GridComm, ProcRank, 2, coords);
    int send_coords_A[2], recv_coords_A[2], send_coords_B[2], recv_coords_B[2], send_rank_A, recv_rank_A, send_rank_B, recv_rank_B;

    MPI_Cart_shift(GridComm, 1, coords[0], &recv_rank_A, &send_rank_A);
    MPI_Cart_shift(GridComm, 0, coords[1], &recv_rank_B, &send_rank_B);

    int* recv_vecA;
    int* recv_vecB;
    recv_vecA = (int *)malloc(sizeof(int) * size * size);
    recv_vecB = (int *)malloc(sizeof(int) * size * size);

    MPI_Cart_coords(GridComm, send_rank_A, 2, send_coords_A);
    MPI_Cart_coords(GridComm, recv_rank_A, 2, recv_coords_A);

    MPI_Cart_coords(GridComm, send_rank_B, 2, send_coords_B);
    MPI_Cart_coords(GridComm, recv_rank_B, 2, recv_coords_B);

    //printf("B: rank: send: %d: %d %d, cur: %d: %d %d, recv: %d: %d %d OK\n", send_rank_B, send_coords_B[0], send_coords_B[1], ProcRank, coords[0], coords[1], recv_rank_B, recv_coords_B[0], recv_coords_B[1]);
    //printf("A: rank: send: %d: %d %d, cur: %d: %d %d, recv: %d: %d %d OK\n", send_rank_A, send_coords_A[0], send_coords_A[1], ProcRank, coords[0], coords[1], recv_rank_A, recv_coords_A[0], recv_coords_A[1]);

    MPI_Cart_shift(GridComm, 0, 1, &recv_rank_B, &send_rank_B);
    MPI_Cart_shift(GridComm, 1, 1, &recv_rank_A, &send_rank_A);

    MPI_Cart_coords(GridComm, send_rank_B, 2, send_coords_B);
    MPI_Cart_coords(GridComm, recv_rank_B, 2, recv_coords_B);
    MPI_Cart_coords(GridComm, send_rank_A, 2, send_coords_A);
    MPI_Cart_coords(GridComm, recv_rank_A, 2, recv_coords_A);

    //printf("B rank: send: %d: %d %d, cur: %d: %d %d, recv: %d: %d %d OK\n", send_rank_B, send_coords_B[0], send_coords_B[1], ProcRank, coords[0], coords[1], recv_rank_B, recv_coords_B[0], recv_coords_B[1]);
    //printf("A rank: send: %d: %d %d, cur: %d: %d %d, recv: %d: %d %d OK\n", send_rank_A, send_coords_A[0], send_coords_A[1], ProcRank, coords[0], coords[1], recv_rank_A, recv_coords_A[0], recv_coords_A[1]);
    
    int* res;
    res = (int *)malloc(sizeof(int) * size * size);
    
    for (int i = 0; i < size * size; ++i) {
        res[i] = 0;
    }

    for (int i = 0; i < mpi_comm_size; ++i) {
        matmul(vecA, vecB, res, size);

        MPI_Sendrecv(vecB, size * size, MPI_INT, send_rank_B, 0, recv_vecB, size * size, MPI_INT, recv_rank_B, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Sendrecv(vecA, size * size, MPI_INT, send_rank_A, 0, recv_vecA, size * size, MPI_INT, recv_rank_A, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < size * size; ++i) {
            vecB[i] = recv_vecB[i];
            vecA[i] = recv_vecA[i];
        } 
    }

    if (isRoot) {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                result.array[i][j] = res[j + i*size];
            }
        }
        int coords[2] = {0, 0};
        int rank;
        for (coords[0] = 0; coords[0] < mpi_comm_size; ++coords[0]) {
            for (coords[1] = 0; coords[1] < mpi_comm_size; ++coords[1]) {
                MPI_Cart_rank(GridComm, coords, &rank);
                if (rank == 0) continue;

                MPI_Recv(res, size * size, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int i = 0; i < size; ++i) {
                    for (int j = 0; j < size; ++j) {
                        result.array[i + coords[0]*size][j + coords[1]*size] = res[j + i*size];
                    }
                }
            }
        }
    } else {
        MPI_Send(res, size*size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (isRoot){
        write_file(argv[3], result);
        free(matrixs.first.array);
        free(matrixs.second.array);
        free(result.array);
    }

    free(recv_vecA);
    free(recv_vecB);
    free(res);
    free(vecA);
    free(vecB);


    double end = MPI_Wtime();
    MPI_Finalize();

    if (isRoot){
        printf("%f\n", end - start);
    }

    return 0;
}