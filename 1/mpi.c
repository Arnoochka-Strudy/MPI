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
    vector_t vec;

    if (isRoot){
        vec = read_file(argv[1]);
    }

    MPI_Bcast(&vec.max_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (!isRoot) vec = init_vector(vec.max_size);
    vec.size = vec.max_size;
    MPI_Bcast(vec.array, vec.size, MPI_INT, 0, MPI_COMM_WORLD);

    int i1, i2;
    i1 = vec.size * ProcRank / ProcNum;
    if (ProcRank == ProcNum - 1) i2 = vec.size;
    else i2 = vec.size * (ProcRank + 1) / ProcNum;

    int local_max = vec.array[0], local_min = vec.array[0];
    int global_max, global_min, el;

    for (int i = i1; i < i2; ++i){
        el = vec.array[i];
        if (local_max < el) local_max = el;
        if (local_min > el) local_min = el;
    }

    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (isRoot) {
        write_file(argv[2], global_max, global_min);
    }

    double end = MPI_Wtime();
    MPI_Finalize();

    free(vec.array);

    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;
}