#!/bin/sh

module load openmpi
module load gcc/9

mpicc -o mpiSimple mpi.c
mpicc -o mpiSendRecv mpi_sendrecv.c
mpicc -o mpiSendRecvReplace mpi_sendrecv_replace.c

rm -f resultsSimple.log
rm -f resultsSendRecv.log
rm -f resultsSendRecvReplace.log

touch resultsSimple.log
touch resultsSendRecv.log
touch resultsSendRecvReplace.log

for i in $(seq 1 9); do
    size=$((10**i))
    mpirun -np 2 ./mpiSimple $size 10 >> resultsSimple.log 2>&1
    mpirun -np 2 ./mpiSendRecv $size 10 >> resultsSendRecv.log 2>&1
    mpirun -np 2 ./mpiSendRecvReplace $size 10 >> resultsSendRecvReplace.log 2>&1
done

rm -f mpiSimple mpiSendRecv mpiSendRecvReplace
