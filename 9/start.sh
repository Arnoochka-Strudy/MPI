#!/bin/sh

module load openmpi
module load gcc/9

declare -A operations=(
    ["Broadcast"]=1
    ["Reduce"]=2
    ["Scatter"]=3
    ["Gather"]=4
    ["AlltoAll"]=5
    ["AllGather"]=6
)

# 43243200 - All
# 4324320 - Reduce

size=43243200
op="$1"
opIdx=${operations[$op]} 

if [ $# -eq 0 ]; then
    exit 1;
else 
    mpicc -o main main.c
    rm -f results$op-0.log results$op-1.log
    touch results$op-0.log results$op-1.log
fi

for (( i=2; i <= 32; i+=2 ))
do
    for (( isMPI=0; isMPI <= 1; isMPI++ ))
    do
        mpirun -np $i ./main $opIdx $isMPI $size >> results$op-$isMPI.log 2>&1  
    done
done

rm -f main

