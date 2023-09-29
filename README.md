<div align="center">
	<img src="http://socsci.uci.edu/~jkrichma/CARL-Logo-small.jpg" width="300"/>
</div>

# CARLsim 6

## Prerequisites

CARLsim 6 comes with the following requirements:
- CMake 3.20 or higher 
- CUDA Toolkit 11.0 or higher. For platform-specific CUDA installation instructions, please navigate to 
  the [NVIDIA CUDA Zone](https://developer.nvidia.com/cuda-zone).
  This is only required if you want to run CARLsim in `GPU_MODE`. Make sure to install the 
  CUDA samples, too, as CARLsim relies on the file `helper_cuda.h`.
- (optional) A GPU with compute capability 6.0 or higher. To find the compute capability of your device please 
  refer to the [CUDA article on Wikipedia](http://en.wikipedia.org/wiki/CUDA).
  This is only required if you want to run CARLsim in `GPU_MODE`.
- (optional) MATLAB R2014a or Octave. This is only required if you want to use the Offline Analysis Toolbox (OAT).

If the Prerequisites cannot be met consider using a former version like CARLsim5 or CARLsim4.


The latest release was tested on the following platforms:
Linux:  Ubuntu 20.04 LTS  
Windows: Windows 10 Professional, Windows 11 Education  
Mac OS X:   
CUDA: 11.2, 11.4, 11.5  
GPUs: Titan Xp, 1080ti, RTX 3090, A100  
  
...  
  
# Use of this branch

This is the CARLsim6 branch known as "ca3net" or "Hippocampome" or "connection specific short-term plasticity" ("CSTP"). Some further development may be wanted on this branch before it could be merged with the main CARLsim6 branch. It can be used in a way to produce correct results with CSTP and some Hippocampome.org properties but some issues exist with this branch. Some conditions that should be known about using this branch are:

<br>* COBA is enabled by default and should always be used (see [issue #27](https://github.com/UCI-CARL/CARLsim6/issues/27)).
<br>* Do not include setConductances() in a line of code (see [issue #27](https://github.com/UCI-CARL/CARLsim6/issues/27)).
<br>* setSTP() needs to be set to "true" for any synaptic connection to work (see [issue #27](https://github.com/UCI-CARL/CARLsim6/issues/27)). The 9-parameter and not 3-parameter version of setSTP() should be used.
<br>* Spike generator code may not work (see [issue #29](https://github.com/UCI-CARL/CARLsim6/issues/29)).
<br>* If using a refractory period (e.g., setting its value to 1 or more in setNeuronParameters()) then a user should only use the RUNGE_KUTTA4 integration method (see [issue #24](https://github.com/UCI-CARL/CARLsim6/issues/24)).
<br>* In connect() and other synaptic connection statements a user should only use a delay of 1 (see [issue #20](https://github.com/UCI-CARL/CARLsim6/issues/20)). Setting connection delays greater than one are allowed in the CSTP branch unlike in the main branch CARLsim code. However, there is indicated to be bugs with any delay greater than 1 so that should not be used.
<br>* Grid3D neuron sizes are not processed correctly (see [issue #18](https://github.com/UCI-CARL/CARLsim6/issues/18)).
<br>* Only the GPU version of CARLsim's processing should be used. For example, when including sim(), a user should only use GPU_MODE not CPU_MODE (see [issue #18](https://github.com/UCI-CARL/CARLsim6/issues/18)). Code for the Hippocampome features has only been developed for GPUs and will not work correctly with CPU.
<br>* Only use CARLsim6 not a different CARLsim version (see [issue #18](https://github.com/UCI-CARL/CARLsim6/issues/18)). The code has not been developed for any other version.

# Setup CARLsim6 as data scientist

## Preliminaries

This setup guide is intended for data scientists using Linux. 
Usually the models and experiments are developed on a workstation having a NVIDIA GeForce
and to be evaluated later on a supercomputer like the DGX A100.
The following preliminaries are derived from the NVIDIA documentation for CUDA 11.5:
1. Linux: Ubuntu 20.04 LTS
2. cMake: 3.22 
3. Google Test: 1.11 

In this guide, the following file structure is used as a reference for the local development. 
Please replace the placeholder user1 with the actual user name:
``` 
/home/user1/
	carlsim6/    # local installation CARLsim6
		includes/
		lib/
		samples/
	cmake-3.22/  # local installation of cMake
		bin/
		share/
	gtest-1.11/  # local installation of Google Test
		inclues/
		lib/
	git/         # cloned repositories from Github
		featCARLsim6/	  
		googletest/	  
```	


## Setup Ubuntu

Ideally Ubuntu 20.04 LTS Desktop is installed from scratch on the workstation. 
For CUDA 11.5 please follow the official [setup guides](https://developer.nvidia.com/cuda-downloads?target_os=Linux&target_arch=x86_64&Distribution=Ubuntu&target_version=20.04&target_type=deb_local).
Also the user should be setup from scratch to avoid any side effects.
While it is technically possible to use multiple CUDA and CARLsim versions side by side 
and switching between them utilizing some kind of `setenv.sh` script, 
such szenarios also depend strongly on the specific requirements and are therefore out of scope. 
A dedicated environment is furthermore essential to find the root cause of potential issues.  

Prepare the .bashrc like the following (replace user1 with the actual user name). 
```
export PATH=/home/user1/cmake-3.22/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/home/user1/gtest-1.11/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
export LD_LIBRARY_PATH=/home/user1/carlsim6/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
```

Validate that the CUDA 11.5 installation has added the following lines:
```
export PATH=/usr/local/cuda-11.5/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-11.5/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
```

Install *pthreads* from the distribution: `sudo apt-get install libpthreads-dev`


## Setup cMake

Download the latest binary (e.g. 3.22) from Kitware and install it to `/home/user1/cMake-3.22`.

Restart the terminal and validate the installation with `cmake --version`.
```
$ cmake --version
cmake version 3.22.0-rc2
CMake suite maintained and supported by Kitware (kitware.com/cmake).
```


## Setup Google Test

Clone the latest stable version (e.g. 1.11) of [Googletest at GitHub](https://github.com/google/googletest) and build it from source.

```
cd ~/git
git clone https://github.com/google/googletest.git
cd googletest
mkdir .build
cd .build
cmake -DCMAKE_INSTALL_PREFIX=/home/user1/gtest-1.11 -DBUILD_SHARED_LIBS=1 -DGTEST_HAS_PTHREAD=1 -DBUILD_GMOCK=OFF ../.
make install
```


## Setup CARLsim6

Until the official release, CARLsim6 is a feature branch in the CARLsim5 repository. 

```
cd ~/git
git clone https://github.com/UCI-CARL/CARLsim5.git featCARLsim6
git checkout feat/CARLsim6
cd featCARLsim6
mkdir .build
cd .build
cmake -DCMAKE_INSTALL_PREFIX=/home/user1/carlsim6 -DCMAKE_BUILD_TYPE=Release ../.
make install
```

> Hint: If cmake does not find the *GTest_DIR* set it manually in cmake-gui to `/home/user1/gtest/lib/cmake/GTest`.
> Also, setting the following environment variables in Linux in ~/.bashrc may help (replace user1 with username):
> ```
> export GTEST_LIBRARY=/home/user1/gtest-1.11/lib/libgtest.a
> export GTEST_MAIN_LIBRARY=/home/user1/gtest-1.11/lib/libgtest_main.a
> export GTEST_ROOT=/home/user1/gtest-1.11/
> ```

Follow the following sequence to repeat builds  
```
make clean
make -j8
make install
```

> Hint: The defaults for cMake are configured to support the latest version of CUDA 
> and the current generation of GeForce graphics card (Ampere achritecture).
> Check which compute capability your your GPU actually has and adopt *CARLSIM_CUDA_GENCODE* accordingly
> either in `cmake-gui` or by passing it as a parameter to the CLI. 
> E.g. for a Titan Xp the parameter is set by `-DCARLSIM_CUDA_GENCODE=-gencode arch=compute_61,code=sm_61`



## Validate the installation

Open a new terminal and validate the settings with `env`.

Start `~/carlsim6/samples/hello_world`

Run the unit tests, e.g.  
```
cd ~/git/featCARLsim6/.build/carlsim/test
./carlsim-tests
```

To run all tests in parallel with monitoring the GPU utilization
```
gnome-terminal -- /bin/sh -c '~/git/featCARLsim6/.build/carlsim/test/carlsim-tests;exec bash' &
gnome-terminal -- /bin/sh -c '~/git/featCARLsim6/.build/carlsim/test6/carlsim-tests6;exec bash' &
gnome-terminal -- /bin/sh -c '~/git/featCARLsim6/.build/carlsim/testadv/carlsim-testsadv --gtest_filter=-*GPU_MultiGPU*;exec bash' &
gnome-terminal -- /bin/sh -c 'nvidia-smi -l 1' &
```



## Running the samples

The executables of the samples are installed to $CMAKE_INSTALL_PREFIX/carlsim6/samples.
Add the path to the .bashrc to repetitive start it from the bash. 
As most of the samples create a result directory or write other files,
create a new working directory to capture the indiviual runs.

Alternative the samples can run directly from the build directory. 
