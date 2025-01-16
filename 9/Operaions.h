#pragma once

#include <mpi.h>
#include <string.h>

typedef int (*Operation)(int**, int*, int, int, int); 

int opSumInt(int** buffers, int* answer, int num_buffers, int i1, int i2) {
    
    for (int i = i1; i < i2; ++i) {
        answer[i - i1] = 0;
        for (int k = 0; k < num_buffers; ++k) {
            answer[i - i1] += buffers[k][i];
        }
    }
}

// Broadcast

int Broadcast(int* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    MPI_Request* requests;
    requests = (MPI_Request *)malloc(sizeof(MPI_Request) * (ProcNum - 1));

    if (ProcRank == root) {
        int wasRoot = 0;
        for (int k = 0; k < ProcNum; ++k) {

            if (root != k){
                MPI_Isend(buffer, count, datatype, k, 0, comm, &requests[k - wasRoot]);
            } else {
                wasRoot = 1;
            }
        }
        MPI_Waitall(ProcNum - 1, requests, MPI_STATUSES_IGNORE);
        free(requests);
    } else {
        MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
    }
}

// Reduce 

int Reduce(const int* sendbuf, int* recvbuf, int count, MPI_Datatype datatype, Operation op, int root, MPI_Comm comm) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int i1, i2, localSize;
    i1 = count * ProcRank / ProcNum;
    if (ProcRank == ProcNum - 1) i2 = count;
    else i2 = count * (ProcRank + 1) / ProcNum;
    localSize = i2 - i1;

    int** buffers;
    int* answer;
    answer = (int *)malloc(sizeof(int) * localSize);
    buffers = (int **)malloc(sizeof(int*) * ProcNum);

    for (int k = 0; k < ProcNum; ++k) {
        buffers[k] = (int *)malloc(sizeof(int) * count);
        if (k == ProcRank){
            for (int i = 0; i < count; ++i) {
                buffers[k][i] = sendbuf[i];
            }
        }
        Broadcast(buffers[k], count, MPI_INT, k, comm);
    }
    op(buffers, answer, ProcNum, i1, i2);

    if (ProcRank == root) {
        for (int k = 0; k < ProcNum; ++k) {

            if (k == root) {
                for (int i = i1; i < i2; ++i) {
                    recvbuf[i] = answer[i - i1];
                    }
            }else {
                i1 = count * k / ProcNum;
                if (k == ProcNum - 1) i2 = count;
                else i2 = count * (k + 1) / ProcNum;
                localSize = i2 - i1;

                MPI_Recv((recvbuf + i1), localSize, datatype, k, 0, comm, MPI_STATUS_IGNORE);
            }
        }
    } else {
        MPI_Send(answer, localSize, datatype, root, 0, comm);
    }
    free(buffers);
    free(answer);

    return 0;
}

// Scatter 

int Scatter(const int* sendbuf, int sendcount, MPI_Datatype sendtype, int* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    MPI_Request* requests;
    requests = (MPI_Request *)malloc(sizeof(MPI_Request) * (ProcNum - 1));

    if (ProcRank == root) {
        int wasRoot = 0;
        for (int k = 0; k < ProcNum; ++k) {
            if (root != k){
                MPI_Isend(sendbuf + (k * sendcount), sendcount, sendtype, k, 0, comm, &requests[k - wasRoot]);
            } else {
                for (int i = 0; i < sendcount; ++i) {
                    recvbuf[i] = sendbuf[k * sendcount + i];
                }
                wasRoot = 1;
            }
        }
        MPI_Waitall(ProcNum - 1, requests, MPI_STATUSES_IGNORE);
        free(requests);

    } else {
        MPI_Recv(recvbuf, recvcount, recvtype, root, 0, comm, MPI_STATUS_IGNORE);
    }

}

// Gather

int Gather(const int* sendbuf, int sendcount, MPI_Datatype sendtype, int* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == root) {
        MPI_Request* requests;
        requests = (MPI_Request *)malloc(sizeof(MPI_Request) * (ProcNum - 1));
        int wasRoot = 0;

        for (int p = 0; p < ProcNum; ++p) {
            if (root != p) {
               MPI_Irecv(recvbuf + (p * recvcount), recvcount, recvtype, p, 0, comm, &requests[p - wasRoot]); 
            } else {
                for (int i = 0; i < sendcount; ++i) {
                    recvbuf[i + root * recvcount] = sendbuf[i];
                }
                wasRoot = 1;
            }
        }
        MPI_Waitall(ProcNum - 1, requests, MPI_STATUSES_IGNORE);
        free(requests);
    } else {
        MPI_Send(sendbuf, sendcount, sendtype, root, 0, comm);
    }
}

// AlltoAll

int AlltoAll(const int* sendbuf, int sendcount, MPI_Datatype sendtype, int* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int wasRoot = 0;
    MPI_Request *SendRequests, *RecvRequests;
    SendRequests = (MPI_Request *)malloc(sizeof(MPI_Request) * (ProcNum - 1));
    RecvRequests = (MPI_Request *)malloc(sizeof(MPI_Request) * (ProcNum - 1));

    for (int p = 0; p < ProcNum; ++p) {
        if (p == ProcRank) {
            for (int i = 0; i < recvcount; ++i) {
                recvbuf[i + p * recvcount] = sendbuf[i + p * sendcount];
                wasRoot = 1;
            }
        } else {
            MPI_Isend(sendbuf  + (p * sendcount), sendcount, sendtype, p, ProcRank, comm, &SendRequests[p - wasRoot]);
            MPI_Irecv(recvbuf  + (p * recvcount), recvcount, recvtype, p, p, comm, &RecvRequests[p - wasRoot]);
        }
    }
    MPI_Waitall(ProcNum - 1, SendRequests, MPI_STATUSES_IGNORE);
    MPI_Waitall(ProcNum - 1, RecvRequests, MPI_STATUSES_IGNORE);
    free(SendRequests);
    free(RecvRequests);
}

// AllGather

int AllGather(const int* sendbuf, int sendcount, MPI_Datatype sendtype, int* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    MPI_Request *SendRequests, *RecvRequests;
    SendRequests = (MPI_Request *)malloc(sizeof(MPI_Request) * (ProcNum - 1));
    RecvRequests = (MPI_Request *)malloc(sizeof(MPI_Request) * (ProcNum - 1));

    for (int i = 0; i < sendcount; ++i) {
        recvbuf[i + ProcRank * recvcount] = sendbuf[i];
    }

    for (int k = 1; k < ProcNum; ++k) {
        MPI_Isend(sendbuf, sendcount, sendtype, (ProcRank + k) % ProcNum, ProcRank, comm, &SendRequests[k - 1]);
        MPI_Irecv(recvbuf + (((ProcRank - k + ProcNum) % ProcNum) * recvcount), recvcount, recvtype, (ProcRank - k + ProcNum) % ProcNum, (ProcRank - k + ProcNum) % ProcNum, comm, &RecvRequests[k - 1]);
    }

    MPI_Waitall(ProcNum - 1, SendRequests, MPI_STATUSES_IGNORE);
    MPI_Waitall(ProcNum - 1, RecvRequests, MPI_STATUSES_IGNORE);
    free(SendRequests);
    free(RecvRequests);
}

