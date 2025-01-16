#!/bin/sh

module load openmpi
module load gcc/9

gcc -fopenmp -o genMat ../Matrix/GenMatrix.c
./genMat 5040 5040 100 A
./genMat 5040 5040 100 B

mpicc -o mpiBuff mpiBuff.c
rm -f resultsSeachBuffSize.log
touch resultsSeachBuffSize.log

for (( i=1; i <= 10; i += 1 ))
do
    mpirun -np 16 ./mpiBuff A B output $i >> resultsSeachBuffSize.log 2>&1
done

rm -f mpiBuff genMat A B output