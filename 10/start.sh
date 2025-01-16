#!/bin/bash

module load openmpi
module load gcc/9

declare -A operations=(
    ["Type"]=1
    ["Pack"]=2
    ["Simple"]=3
) 

for op in "${!operations[@]}";
do
    mpicc -o $op $op.c
    rm -f results$op.log
    touch results$op.log

    for (( i=2; i <= 8; i+=1 ))
    do
        mpirun -np $i ./$op >> results$op.log 2>&1  
    done
    rm -f $op
done

