#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include "Particle.h"

int packParticles(particle_t* particles, int* bufX, int* bufY, double* bufF, char* bufName) {
    for (int i = 0; i < N; ++i) {
        bufX[i] = particles[i].x;
        bufY[i] = particles[i].y;
        bufF[i] = particles[i].f;
        strncpy(bufName + (i * 8), particles[i].name, sizeof(particles[0].name) - 1);
        bufName[i*8 + 7] = '\0';
    }
    return 0;
}

int unpackParticles(int* bufX, int* bufY, double* bufF, char* bufName, particle_t* particles) {
    for (int i = 0; i < N; ++i) {
        particles[i].x = bufX[i];
        particles[i].y = bufY[i];
        particles[i].f = bufF[i];
        strncpy(particles[i].name, bufName + (i * 8), sizeof(particles[0].name) - 1);
        particles[i].name[7] = '\0';
    }
    return 0;
}

int main(int argc, char* argv[]) {

    int ProcNum, ProcRank, isRoot;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    isRoot = ProcRank == 0;

    particle_t* particles;
    particles = (particle_t *)malloc(sizeof(particle_t) * N);

    int position = 0, sizebuf = 0, size;
    MPI_Pack_size(2, MPI_INT, MPI_COMM_WORLD, &size);
    sizebuf += size;
    MPI_Pack_size(1, MPI_DOUBLE, MPI_COMM_WORLD, &size);
    sizebuf += size;
    MPI_Pack_size(8, MPI_CHAR, MPI_COMM_WORLD, &size);
    sizebuf += size;

    sizebuf *= N;
    int *bufX, *bufY;
    double* bufF;
    char* bufName;
    bufX = (int *)malloc(sizeof(int) * N);
    bufY = (int *)malloc(sizeof(int) * N);
    bufF = (double *)malloc(sizeof(double) * N);
    bufName = (char *)malloc(sizeof(char) * N * 8);

    if (isRoot){
        for (int i = 0; i < N; ++i) {
            initTest(&particles[i]);
        }
    }
    double start = MPI_Wtime();
    if (isRoot) {
        packParticles(particles, bufX, bufY, bufF, bufName);
    }
    MPI_Bcast(bufX, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(bufY, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(bufF, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(bufName, N * 8, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (!isRoot) {
        unpackParticles(bufX, bufY, bufF, bufName, particles);
    }
    double end = MPI_Wtime();

    if (isRoot) {
        printf("%f\n", end - start);
    }

    MPI_Finalize();

    return 0;
}