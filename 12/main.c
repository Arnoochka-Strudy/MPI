#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "Topologies.h"



int main(int argc, char* argv[]) {

    if (argc < 2) {
        return 1;
    }

    int ProcNum, ProcRank, isRoot;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    isRoot = ProcRank == 0;
    MPI_Comm Comm;

    int topology = atoi(argv[1]);
    
    printf("OK\n");
    switch (topology)
    {
    case 1: {
            int ndims, *dims, *periods, InputProcNum = 1;
            ndims = atoi(argv[2]);
            if (argc < ndims + 3) {
                return 1;
            }
            dims = (int *)malloc(sizeof(int) * ndims);
            periods = (int *)malloc(sizeof(int) * ndims);
            for (int i = 0; i < ndims; ++i) {
                dims[i] = atoi(argv[i + 3]);
                periods[i] = 0;
            }
            createCartTopology(MPI_COMM_WORLD, &Comm, ndims, dims);
        }
        break;

    case 2: {
            int ndims, *dims, *periods, InputProcNum = 1;
            ndims = atoi(argv[2]);
            if (argc < ndims + 3) {
                return 1;
            }
            dims = (int *)malloc(sizeof(int) * ndims);
            periods = (int *)malloc(sizeof(int) * ndims);
            for (int i = 0; i < ndims; ++i) {
                dims[i] = atoi(argv[i + 3]);
                periods[i] = 0;
            }
            createCartTopology(MPI_COMM_WORLD, &Comm, ndims, dims);
        }
        break;

    case 3: {
            int root;
            if (argc < 3) {
                return 1;
            }
            root = atoi(argv[2]);
            createStarTopology(MPI_COMM_WORLD, &Comm, root);
        }
        break;
    
    default:
        printf("uncnown command\n");
        break;
    }

    MPI_Comm_free(&Comm);
    MPI_Finalize();
    return 0;
}