#!/bin/sh

module load openmpi
module load gcc/9

mpicc -o mpi mpi.c

rm -f results.log
touch results.log

for i in $(seq 1 5); do
    size=$((10**i))
    mpirun -np 2 ./mpi $size 10 >> results.log 2>&1
done

rm -f mpi genVec input
