#!/bin/sh

module load openmpi
module load gcc/9

gcc -fopenmp -o genMat ../Matrix/GenMatrix.c
./genMat 1000 1000 100 A
./genMat 1000 1000 100 B

# gcc -o simple simple.c
# simple=$(./simple A B output 2>&1)
# echo "$simple" >> resultsSimple.log 2>&1
# echo "$simple" >> resultsReady.log 2>&1
# echo "$simple" >> resultsSync.log 2>&1
# echo "$simple" >> resultsMPI_Recv(result.array[proc_id * size[0] + idx], size[1], MPI_INT, proc_id, idx, MPI_COMM_WORLD, MPI_STATUS_IGNORE);Buff.log 2>&1

mpicc -o mpiSimple mpiSimple.c
mpicc -o mpiReady mpiReady.c
mpicc -o mpiSync mpiSync.c
mpicc -o mpiBuff mpiBuff.c

rm -f resultsSimple.log
rm -f resultsReady.log
rm -f resultsSync.log
rm -f resultsBuff.log

touch resultsSimple.log
touch resultsReady.log
touch resultsSync.log
touch resultsBuff.log

for (( i=2; i <= 2; i++ ))
do
    let np=$i*$i
    echo "$np"
    # mpirun -np $np ./mpiSimple A B output >> resultsSimple.log 2>&1
    mpirun -np $np ./mpiReady A B output >> resultsReady.log 2>&1
    # mpirun -np $np ./mpiSync A B output >> resultsSync.log 2>&1
    # mpirun -np $np ./mpiBuff A B output 3 >> resultsBuff.log 2>&1
done

rm -f simple mpiSimple mpiReady mpiBuff mpiSync output A B genMat

