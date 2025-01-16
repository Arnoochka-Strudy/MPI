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
    pair_t vecs;

    if (isRoot){
        vecs = read_file(argv[1], ProcNum);
    }

    MPI_Bcast(&vecs.size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (!isRoot) {
        vecs.first = init_vector(vecs.size);
        vecs.second = init_vector(vecs.size);
        vecs.first.size = vecs.size;
        vecs.second.size = vecs.size;
    }

    MPI_Bcast(vecs.first.array, vecs.size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(vecs.second.array, vecs.size, MPI_INT, 0, MPI_COMM_WORLD);

    int i1, i2;
    long long global_result = 0, local_result = 0;
    i1 = vecs.size * ProcRank / ProcNum;
    if (ProcRank == ProcNum - 1) i2 = vecs.size;
    else i2 = vecs.size * (ProcRank + 1) / ProcNum;

    for (int i = i1; i < i2; ++i) {
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