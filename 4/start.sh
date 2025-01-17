#!/bin/sh

module load openmpi
module load gcc/9

gcc -fopenmp -o genMat ../Matrix/GenMatrix.c
./genMat 10000 10000 100 A
./genMat 10000 10000 100 B

gcc -o simple simple.c
mpicc -o mpiS mpiStriped.c
mpicc -o mpiF mpiFox.c -lm

rm -f resultsS.log
rm -f resultsF.log

touch resultsS.log
touch resultsF.log

simple=$(./simple A B output 2>&1)
echo "$simple" >> resultsS.log 2>&1
echo "$simple" >> resultsF.log 2>&1

for (( i=2; i <= 12; i++ ))
do
    mpirun -np $(( $i*$i )) ./mpiS A B output >> resultsS.log 2>&1
    mpirun -np $(( $i*$i )) ./mpiF A B output >> resultsF.log 2>&1
done

rm -f simple mpiS mpiF genMat output A B  genMat

