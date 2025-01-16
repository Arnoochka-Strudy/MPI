#pragma once

#include <string.h>
#include <mpi.h>

#define N 28828800

typedef struct Particle
{
    int x;
    int y;
    double f;
    char name[8];

} particle_t;

int print_particle(particle_t* particle) {
    printf("name: ");
    for (int i = 0; i < 7; ++i) {
        printf("%c", particle->name[i]);
    }
    printf("%c, [x: %d, y: %d, f: %.2f]\n", particle->name[7], particle->x, particle->y, particle->f);
}

int initTest(particle_t* particle) {
    particle->x = 42;
    particle->y = 69;
    particle->f = 42.69;
    strncpy(particle->name, "VictorK", sizeof(particle->name) - 1);
    particle->name[sizeof(particle->name) - 1] = '\0';
    return 0;
}