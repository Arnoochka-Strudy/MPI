#pragma once

#include <mpi.h>
#include <stdlib.h>

int createCartTopology(MPI_Comm OldComm, MPI_Comm* NewComm, int ndims, int* dims) {
    int InputProcNum = 1, ProcNum, *periods;
    periods = (int *)malloc(sizeof(int) * ndims);
    MPI_Comm_size(OldComm, &ProcNum);

    for (int i = 0; i < ndims; ++i) {
        periods[i] = 0;
        InputProcNum *= dims[i];
    }
    if (InputProcNum != ProcNum) {
        exit(1);
    } else {
        MPI_Cart_create(OldComm, ndims, dims, periods, 1, NewComm);
        return 0;
    }
}

int createTorTopology(MPI_Comm OldComm, MPI_Comm* NewComm, int ndims, int* dims) {
    int InputProcNum = 1, ProcNum, *periods;
    periods = (int *)malloc(sizeof(int) * ndims);
    MPI_Comm_size(OldComm, &ProcNum);

    for (int i = 0; i < ndims; ++i) {
        periods[i] = 1;
        InputProcNum *= dims[i];
    }
    if (InputProcNum != ProcNum) {
        exit(1);
    } else {
        MPI_Cart_create(OldComm, ndims, dims, periods, 1, NewComm);
    }
}

int createStarTopology(MPI_Comm OldComm, MPI_Comm* NewComm, int root) {
    int ProcNum, ProcRank, isRoot;
    MPI_Comm_rank(OldComm, &ProcRank);
    MPI_Comm_size(OldComm, &ProcNum);
    isRoot = ProcRank == root;

    int *index, *edges;

    index = (int *)malloc(sizeof(int) * ProcNum);
    edges = (int *)malloc(sizeof(int) * (ProcNum - 1) * 2);

    for (int i = 0; i < ProcNum; ++i){
        if (root == i) index[i] = ProcNum - 1;
        else index[i] = 1;
    }

    int wasRoot = 0;
    for (int i = 0; i < ProcNum ; ++i) {
        if (root == i)wasRoot = 1;
        else edges[root + i - wasRoot] = i;
    }
    for (int i = 0; i < root; ++i) edges[i] = root;
    for (int i = root + ProcNum - 1; i < 2 * (ProcNum - 1); ++i) edges[i] = root;
    MPI_Graph_create(OldComm, ProcNum, index, edges, 1, NewComm);

}