#!/bin/bash
#PBS -l walltime=00:01:30,nodes=3:ppn=2
#PBS -N Boriskova
#PBS -q batch
cd $PBS_O_WORKDIR
mpirun  --hostfile $PBS_NODEFILE -np 6 ./a.out
