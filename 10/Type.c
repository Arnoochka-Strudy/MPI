#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "Particle.h"

MPI_Datatype getMPITypeParticle() {
    MPI_Datatype PARTICLE;
    MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_DOUBLE, MPI_CHAR};
    int block_lengths[4] = {1, 1, 1, 8};
    MPI_Aint offsets[4];
    offsets[0] = offsetof(particle_t, x);
    offsets[1] = offsetof(particle_t, y);
    offsets[2] = offsetof(particle_t, f);
    offsets[3] = offsetof(particle_t, name);

    MPI_Type_create_struct(4, block_lengths, offsets, types, &PARTICLE);
    MPI_Type_commit(&PARTICLE);
    return PARTICLE;
}

int main(int argc, char* argv[]) {

    int ProcNum, ProcRank, isRoot;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    isRoot = ProcRank == 0;

    particle_t* particles;
    particles = (particle_t *)malloc(sizeof(particle_t) * N);

    if (isRoot){
        for (int i = 0; i < N; ++i) {
            initTest(&particles[i]);
        }
    }
    double start = MPI_Wtime();
    MPI_Datatype PARTICLE = getMPITypeParticle();
    MPI_Bcast(particles, N, PARTICLE, 0, MPI_COMM_WORLD);
    MPI_Type_free(&PARTICLE);
    double end = MPI_Wtime();

    if (isRoot) {
        printf("%f\n", end - start);
    }
    MPI_Finalize();

    return 0;
}