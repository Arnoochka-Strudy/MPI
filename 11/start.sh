#!/bin/bash

module load openmpi
module load gcc/9

mpicc -o main main.c

rm -f results-0.log results-1.log
touch results-0.log results-1.log

for (( row = 1; row <= 8; row++ ))
do
    for (( column = 2; column <= 32; column+=2 ))
    do
        if (( row * column > 8 )); then
            break
        fi

        let np=$row*$column
        for (( isGrid = 0; isGrid <= 1; isGrid++ ))
        do
            echo "mpirun -np $np ./main 2 $row $column $isGrid >> results-$isGrid.log 2>&1"
            mpirun -np $np ./main 2 $row $column $isGrid >> results-$isGrid.log 2>&1
        done
    done
done

rm -f main

