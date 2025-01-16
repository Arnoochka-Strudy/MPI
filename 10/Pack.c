#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "Particle.h"

int packParticle(particle_t* particle, void* buf, int sizebuf, int* position, MPI_Comm comm) {
    MPI_Pack(&(particle->x), 1, MPI_INT, buf, sizebuf, position, comm);
    MPI_Pack(&(particle->y), 1, MPI_INT, buf, sizebuf, position, comm);
    MPI_Pack(&(particle->f), 1, MPI_DOUBLE, buf, sizebuf, position, comm);
    MPI_Pack(particle->name, 8, MPI_CHAR, buf, sizebuf, position, comm);
    return 0;
}

int unpackParticle(void* buf, int sizebuf, int* position, particle_t* particle, MPI_Comm comm) {
    MPI_Unpack(buf, sizebuf, position, &(particle->x), 1, MPI_INT, comm);
    MPI_Unpack(buf, sizebuf, position, &(particle->y), 1, MPI_INT, comm);
    MPI_Unpack(buf, sizebuf, position, &(particle->f), 1, MPI_DOUBLE, comm);
    MPI_Unpack(buf, sizebuf, position, &(particle->name), 8, MPI_CHAR, comm);
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
    void* buf;
    buf = malloc(sizebuf);

    if (isRoot){
        for (int i = 0; i < N; ++i) {
            initTest(&particles[i]);
        }
    }
    double start = MPI_Wtime();
    if (isRoot) {
        for (int i = 0; i < N; ++i) {
            packParticle(&particles[i], buf, sizebuf, &position, MPI_COMM_WORLD);
        }
    }
    MPI_Bcast(&position, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(buf, position, MPI_PACKED, 0, MPI_COMM_WORLD);

    if (!isRoot) {
        position = 0;
        for (int i = 0; i < N; ++i) {
            unpackParticle(buf, sizebuf, &position, &particles[i], MPI_COMM_WORLD);
        }
    }
    double end = MPI_Wtime();

    if (isRoot) {
        printf("%f\n", end - start);
    }

    MPI_Finalize();

    return 0;
}