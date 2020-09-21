#!/bin/bash
#PBS -l walltime=00:01:00,nodes=3:ppn=1
#PBS -N Boriskova
#PBS -q batch
cd $PBS_O_WORKDIR
mpirun  --hostfile $PBS_NODEFILE -np 3 ./teplo_parall.out 1000
