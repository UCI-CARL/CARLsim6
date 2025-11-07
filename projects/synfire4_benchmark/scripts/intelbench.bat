@ECHO OFF
echo benchmark synfire4 snn for %2 seconds 6x:

SET OMP_PROC_BIND=TRUE
@REM SET OMP_NUM_THREADS=4
@REM export OMP_PLACES={0},{1},{2},{3}
@REM SET OMP_PLACES="{0},{2},{4},{6}"
@REM export OMP_PLACES={1},{3},{5},{7}
@REM export OMP_PLACES={4},{5},{6},{7}

@echo synfire4 %1 1 cores >&2
.\synfire4  --carlsim_rand_seed=42 --carlsim_custom_2=%2 --carlsim_omp_threads=1 >"results\console-synfire,omp,%1,1cores.txt"
@echo done.  >&2
@timeout 5   >&2
 
@echo synfire4 %1 2 cores
.\synfire4   --carlsim_rand_seed=42 --carlsim_custom_2=%2 --carlsim_omp_threads=2 >"results\console-synfire,omp,%1,2cores.txt"
@echo done.  >&2
@timeout 5  >&2

@REM @echo synfire4 %1 3 cores
@REM .\synfire4   --carlsim_rand_seed=42 --carlsim_custom_2=%2 --carlsim_omp_threads=3 >"results\console-synfire,omp,%1,3cores.txt"
@REM @echo done.  >&2
@REM @timeout 5  >&2


@echo synfire4 %1 4 cores
.\synfire4  --carlsim_rand_seed=42 --carlsim_custom_2=%2 --carlsim_omp_threads=4 >"results\console-synfire,omp,%1,4cores.txt"
@echo done.  >&2
@timeout 5  >&2

@echo synfire4 %1 8 cores
.\synfire4  --carlsim_rand_seed=42 --carlsim_custom_2=%2 --carlsim_omp_threads=8 >"results\console-synfire,omp,%1,8cores.txt"
@echo done.  >&2
@timeout 5  >&2

@echo synfire4 %1 16 cores
.\synfire4  --carlsim_rand_seed=42 --carlsim_custom_2=%2 --carlsim_omp_threads=16 >"results\console-synfire,omp,%1,16cores.txt"
@echo done.  >&2
@timeout 5  >&2



