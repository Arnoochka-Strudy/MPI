#include <stdio.h>
#include "../Vector/vector.h"
#include "ReadWrite.h"
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        return 1;
    }

    int ProcNum, ProcRank, RecvRank;
    MPI_Init(&argc, &argv);
    double start = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int isRoot = ProcRank == 0;
    pair_t data, vecs;

    if (isRoot){
        data = read_file(argv[1], ProcNum);
    }

    MPI_Bcast(&data.size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vecs.first = init_vector(data.size / ProcNum);
    vecs.second = init_vector(data.size / ProcNum);
    vecs.first.size = data.size / ProcNum;
    vecs.second.size = data.size / ProcNum;
    vecs.size = data.size / ProcNum;
    
    MPI_Scatter(data.first.array, data.size / ProcNum, MPI_INT, vecs.first.array, data.size / ProcNum, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(data.second.array, data.size / ProcNum, MPI_INT, vecs.second.array, data.size / ProcNum, MPI_INT, 0, MPI_COMM_WORLD);

    long long global_result = 0, local_result = 0;

    for (int i = 0; i < vecs.size; ++i) {
        local_result += vecs.first.array[i] * vecs.second.array[i];
    }

    MPI_Reduce(&local_result, &global_result, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);


    if (isRoot){
        write_file(argv[2], global_result);
    }

    double end = MPI_Wtime();
    MPI_Finalize();

    free(vecs.first.array);
    free(vecs.second.array);

    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;
}