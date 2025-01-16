#include <stdio.h>
#include "../Vector/vector.h"
#include <time.h>
#include <mpi.h>
#include <unistd.h>

void computation(double seconds) {
    usleep(seconds);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        return 1;
    }

    int ProcNum, ProcRank, RecvRank;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int isRoot = ProcRank == 0;

    int dimension = atoi(argv[3]);
    double seconds = atof(argv[1]) / ProcNum * dimension;
    int size_comm = atoi(argv[2]) * dimension;
    vector_t vec = init_vector(size_comm);
    vector_t recv_vec = init_vector(size_comm);

    if (isRoot){
        for (int i = 0; i < size_comm; ++i) {
            vec.array[i] = rand() % 10;
        }

    }

    double start = MPI_Wtime();

    MPI_Bcast(vec.array, size_comm, MPI_INT, 0, MPI_COMM_WORLD);

    for (int k = 0; k < ProcNum; ++k) {
        computation(seconds);
        MPI_Sendrecv(vec.array, size_comm, MPI_INT, (ProcRank + 1) % ProcNum, 0, recv_vec.array, size_comm, MPI_INT, (ProcNum + ProcRank - 1) % ProcNum, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int idx = 0; idx < size_comm; ++idx) {
            vec.array[idx] = recv_vec.array[idx];
        }
    }

    double end = MPI_Wtime();

    free(vec.array);
    free(recv_vec.array);

    MPI_Finalize();
    if (isRoot){
        printf("%d %d %f\n", (int)(seconds * ProcNum / dimension), size_comm / dimension, end - start);
    }
    return 0;
}