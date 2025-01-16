#!/bin/sh
module load openmpi 
module load gcc/9 

gcc -o genVec ../Vector/GenVectors.c
./genVec 2 86486400 10000 input

gcc -o simple simple.c
mpicc -o mpiBcast mpiBcast.c
mpicc -o mpiScatter mpiScatter.c

rm -f resultsBcast.log resultsScatter.log
touch resultsBcast.log resultsScatter.log

simple=$(./simple input output 2>&1)
echo "$simple" >> resultsBcast.log 2>&1
echo "$simple" >> resultsScatter.log 2>&1

for (( i=2; i <= 32; i+= 2 ))
do
    mpirun -np $i ./mpiBcast input output >> resultsBcast.log 2>&1
    mpirun -np $i ./mpiScatter input output >> resultsScatter.log 2>&1
done

rm -f mpiBcast mpiScatter simple genVec input output
