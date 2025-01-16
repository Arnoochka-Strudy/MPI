#include <stdlib.h>
#include <stdio.h> 
#include <mpi.h>
#include "Operaions.h"
#include "../Vector/vector.h"

int broadcastTest(int ProcNum, int ProcRank, int argc, char* argv[]) {
    int isMPI, size, isRoot;
    if (argc == 3){
        isMPI = atoi(argv[1]);
        size = atoi(argv[2]);
        isRoot = ProcRank == 0;
    } else {
        return 1;
    }
    vector_t vec = init_vector(size);
    vec.size = vec.max_size;

    if (isRoot){
        for (int i = 0; i < size; ++i) {
            vec.array[i] = rand() % 10;
            }
        }

    double start = MPI_Wtime();
    if (isMPI) MPI_Bcast(vec.array, size, MPI_INT, 0, MPI_COMM_WORLD);
    else Broadcast(vec.array, size, MPI_INT, 0, MPI_COMM_WORLD);
    double end = MPI_Wtime();
    free(vec.array);

    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;

}

int reduceTest(int ProcNum, int ProcRank, int argc, char* argv[]) {
    int isMPI, size, isRoot;
    if (argc == 3){
        isMPI = atoi(argv[1]);
        size = atoi(argv[2]);
        isRoot = ProcRank == 0;
    } else {
        return 1;
    } 

    vector_t local, global;

    local = init_vector(size);
    local.size = local.max_size;

    if (isRoot){
        global = init_vector(size);
        global.size = global.max_size;
    }

    for (int i = 0; i < size; ++i) {
        local.array[i] = rand() % 10;
    }

    double start = MPI_Wtime();
    if (isMPI) MPI_Reduce(local.array, global.array, size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    else Reduce(local.array, global.array, size, MPI_INT, opSumInt, 0, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    free(local.array);
    if (isRoot) free(global.array);

    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;

}

int scatterTest(int ProcNum, int ProcRank, int argc, char* argv[]) {
    int isMPI, size, isRoot;
    if (argc == 3){
        isMPI = atoi(argv[1]);
        size = atoi(argv[2]);
        isRoot = ProcRank == 0;
    } else {
        return 1;
    }

    vector_t svec, rvec;
    int count = size / ProcNum;

    if (isRoot){
        svec = init_vector(size);
        svec.size = svec.max_size;
        for (int i = 0; i < size; ++i) {
            svec.array[i] = rand() % 10;
            }
    }

    rvec = init_vector(count);
    rvec.size = rvec.max_size;

    double start = MPI_Wtime();
    if (isMPI) MPI_Scatter(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, 0, MPI_COMM_WORLD);
    else Scatter(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, 0, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (isRoot) free(svec.array);
    free(rvec.array);

    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;

}

int gatherTest(int ProcNum, int ProcRank, int argc, char* argv[]) {
    int isMPI, size, isRoot;
    if (argc == 3){
        isMPI = atoi(argv[1]);
        size = atoi(argv[2]);
        isRoot = ProcRank == 0;
    } else {
        return 1;
    }

    vector_t svec, rvec;
    int count = size / ProcNum;

    if (isRoot){
        rvec = init_vector(size);
        rvec.size = rvec.max_size;
    }

    svec = init_vector(count);
    svec.max_size = svec.size;

    for (int i = 0; i < count; ++i) {
        svec.array[i] = rand() % 10;
    }

    double start = MPI_Wtime();
    if (isMPI) MPI_Gather(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, 0, MPI_COMM_WORLD);
    else Gather(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, 0, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (isRoot) free(rvec.array);
    free(svec.array);



    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;

}

int alltoallTest(int ProcNum, int ProcRank, int argc, char* argv[]) {
    int isMPI, size, isRoot;
    if (argc == 3){
        isMPI = atoi(argv[1]);
        size = atoi(argv[2]);
        isRoot = ProcRank == 0;
    } else {
        return 1;
    }

    vector_t svec, rvec;
    int count = size / ProcNum;

    rvec = init_vector(size);
    rvec.size = rvec.max_size;
    svec = init_vector(size);
    svec.size = svec.max_size;

    for (int i = 0; i < size; ++i) {
        svec.array[i] = rand() % 10;
    }

    double start = MPI_Wtime();
    if (isMPI) MPI_Alltoall(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, MPI_COMM_WORLD);
    else AlltoAll(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    free(rvec.array);
    free(svec.array);

    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;
}

int allgatherTest(int ProcNum, int ProcRank, int argc, char* argv[]) {
    int isMPI, size, isRoot;
    if (argc == 3){
        isMPI = atoi(argv[1]);
        size = atoi(argv[2]);
        isRoot = ProcRank == 0;
    } else {
        return 1;
    }

    vector_t svec, rvec;
    int count = size / ProcNum;

    rvec = init_vector(size);
    rvec.size = rvec.max_size;
    svec = init_vector(count);
    svec.size = svec.max_size;

    for (int i = 0; i < count; ++i) {
        svec.array[i] = rand() % 10;
    }

    double start = MPI_Wtime();
    if (isMPI) MPI_Allgather(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, MPI_COMM_WORLD);
    else AllGather(svec.array, count, MPI_INT, rvec.array, count, MPI_INT, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    free(rvec.array);
    free(svec.array);

    if (isRoot){
        printf("%f\n", end - start);
    }
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        return 1;
    }

    int ProcNum, ProcRank;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int op = atoi(argv[1]);

    switch (op)
    {
    case 1:
        broadcastTest(ProcNum, ProcRank, argc - 1, argv + 1);
        break;

    case 2:
        reduceTest(ProcNum, ProcRank, argc - 1, argv + 1);
        break;

    case 3:
        scatterTest(ProcNum, ProcRank, argc - 1, argv + 1);  
        break; 

    case 4:
        gatherTest(ProcNum, ProcRank, argc - 1, argv + 1); 
        break;
    
    case 5:
        alltoallTest(ProcNum, ProcRank, argc - 1, argv + 1);
        break;

    case 6:
        allgatherTest(ProcNum, ProcRank, argc - 1, argv + 1); 
        break;
    
    default:
        printf("uncnown command\n");
        break;
    } 
    MPI_Finalize();

    return 0;
}