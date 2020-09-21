#!/bin/bash
#PBS -l walltime=00:01:00,nodes=1:ppn=1
#PBS -N Boriskova
#PBS -q batch
cd $PBS_O_WORKDIR
mpirun  --hostfile $PBS_NODEFILE -np 1 ./teplo.out 10001
