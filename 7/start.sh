#!/bin/sh

module load openmpi
module load gcc/9

mpicc -o mpi mpi.c

if [ $# -lt 1 ]; then
    echo "Usage: $0 <number_of_processes>"
    exit 1
fi

np=$1

rm -f "results_$np.log"
touch "results_$np.log"

for (( seconds=0; seconds<=2; seconds++ )); do  
    for (( size=0; size<=20; size+= 2 )); do
        mpirun -np $np ./mpi $seconds $size 1000000 >> "results_$np.log" 2>&1
    done
done

rm -f mpi
