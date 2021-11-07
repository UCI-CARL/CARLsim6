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
Mac OS X:  
Windows 10 Professional, 11 Education

CUDA: 11.2, 11.4, 11.5

GPUs: A100, RTX 3090ti, Titan Xp, A100, ...  



## Using CMake on Linux Ubunutu  

--> Quick Start: cmake-gui  

1. Clone repositor from GIT 

2. Create a build directory (you can make it anywhere)

   ```
   $ mkdir .build
   ```

3. Proceed into build directory and do configuration:

   ```
   $ cd .build
   $ cmake \
       -DCMAKE_BUILD_TYPE=Release \
       -DCMAKE_INSTALL_PREFIX=/usr/local/carlsim \
       -DCARLSIM_NO_CUDA=OFF \
       <path-to-carlsim>
   ```

   As you can see `cmake` accepts several options `-D<name>=<value>`: they define cmake variables.
   `CMAKE_BUILD_TYPE=Release` means that we are going to build release version of the library.
   If you need debug version then pass `Debug`.
   `CMAKE_INSTALL_PREFIX` specifies a directory which we are going to install the library into.
   `CARLSIM_NO_CUDA` switches on/off support of CUDA inside the library.
   `<path-to-carlsim>` must be replaced with the path to the CARLsim5's source directory.

4. Build:

   ```
   make -j <jobs-num>
   ```
   
   Set `<jobs-num>` to the number of logical processors your computer has plus one,
   this will employ parallel building.

5. Install:

   ```
   sudo make install
   ```
   


