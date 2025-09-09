#!/bin/sh
echo "usage: scripts/pibench.sh i5 20"
echo "benchmark synfire snn for $2 seconds 4x:"
export OMP_PROC_BIND=TRUE
echo "synfire $1 1 cores" >&2
./synfire  --carlsim_rand_seed=42 --carlsim_custom_2=$2 --carlsim_omp_threads=1 >results/console-synfire,omp,$1,1cores.txt
#echo "done,resting for 5s."  >&2
echo "done."  >&2
sleep 5
echo "synfire $1 2 cores"
./synfire  --carlsim_rand_seed=42 --carlsim_custom_2=$2 --carlsim_omp_threads=2 >results/console-synfire,omp,$1,2cores.txt
echo "done."  >&2
sleep 5
echo "synfire $1 3 cores"
./synfire  --carlsim_rand_seed=42 --carlsim_custom_2=$2 --carlsim_omp_threads=3 >results/console-synfire,omp,$1,3cores.txt
echo "done."  >&2
sleep 5
echo "synfire $1 4 cores"
./synfire  --carlsim_rand_seed=42 --carlsim_custom_2=$2 --carlsim_omp_threads=4 >results/console-synfire,omp,$1,4cores.txt
echo "done."  >&2
sleep 5
