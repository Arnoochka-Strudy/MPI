#!/bin/sh
module load openmpi 
module load gcc/9 

gcc -o genVec ../Vector/GenVectors.c
./genVec 1 86486400 10000000 input

gcc -o simple simple.c
mpicc -o mpi mpi.c

rm -f results.log
touch results.log

./simple input output >> results.log 2>&1

for (( i=2; i <= 4; i++ ))
do
    mpirun -np $i ./mpi input output >> results.log 2>&1
done

rm -f mpi simple genVec input output
