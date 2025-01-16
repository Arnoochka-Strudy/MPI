#include <mpi.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

#define N 43243200


int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    int isGrid;
    int ndims, *dims, *periods, InputProcNum = 1;
    ndims = atoi(argv[1]);
    if (argc < ndims + 2) {
        return 1;
    }

    dims = (int *)malloc(sizeof(int) * ndims);
    periods = (int *)malloc(sizeof(int) * ndims);
    for (int i = 0; i < ndims; ++i) {
        dims[i] = atoi(argv[i + 2]);
        periods[i] = 0;
        InputProcNum *= dims[i];
    }

    int ProcNum, ProcRank, isRoot;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    isRoot = ProcRank == 0;

    if (InputProcNum != ProcNum) {
        MPI_Finalize();
        return 1;
    }
    MPI_Comm GridComm;

    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, 1, &GridComm);

    int* coords;
    coords = (int *)malloc(sizeof(int) * ndims);
    MPI_Cart_coords(GridComm, ProcRank, ndims, coords);

    if (ndims == 2 && argc == 5) {
        int isGrid = atoi(argv[4]);

        MPI_Comm RowComm, ColumnComm;
        int row = coords[0], column = coords[1];
        MPI_Comm_split(GridComm, row, column, &RowComm);
        MPI_Comm_split(GridComm, column, row, &ColumnComm);

        int *svec, *rvec;
        svec = (int *)malloc(sizeof(int) * N);
        rvec = (int *)malloc(sizeof(int) * N);

        for (int i = 0; i < N; ++i) {
            svec[i] = rand() % 10;
        }
        double start = MPI_Wtime();
        if (isGrid) MPI_Alltoall(svec, N / ProcNum, MPI_INT, rvec, N / ProcNum, MPI_INT, GridComm);
        else MPI_Alltoall(svec, N / dims[1], MPI_INT, rvec, N / dims[1], MPI_INT, RowComm);
        double end = MPI_Wtime();

        if (isRoot) {
            printf("%d %d %f\n", dims[0], dims[1], end - start);
        }

        MPI_Comm_free(&RowComm);
        MPI_Comm_free(&ColumnComm);
    }

    MPI_Comm_free(&GridComm);
    MPI_Finalize();

    return 0;
} 