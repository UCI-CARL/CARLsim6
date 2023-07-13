<!--![](https://sites.socsci.uci.edu/~jkrichma/.Colored-CARL-Logo.gif)-->
<div align="center">
	<img src="https://sites.socsci.uci.edu/~jkrichma/.Colored-CARL-Logo.gif" width="300"/>
</div>

# CARLsim 6.1

[![Build Status](https://github.com/UCI-CARL/CARLsim6/actions/workflows/build.yml/badge.svg)](https://github.com/bainro/autoCARL/actions/runs)[![Build Status](https://github.com/UCI-CARL/CARLsim6/actions/workflows/binaries.yml/badge.svg)](https://github.com/bainro/autoCARL/actions/runs)
[![Coverage Status](https://coveralls.io/repos/github/UCI-CARL/CARLsim6/badge.svg?branch=master)](https://coveralls.io/github/UCI-CARL/CARLsim6?branch=master)
[![Docs](https://img.shields.io/badge/docs-v6.0.0-blue.svg)](http://uci-carl.github.io/CARLsim6)
[![Google group](https://img.shields.io/badge/Google-Discussion%20group-blue.svg)](https://groups.google.com/forum/#!forum/carlsim-snn-simulator)

CARLsim is an efficient, easy-to-use, GPU-accelerated library for simulating large-scale spiking neural network (SNN) models 
with a high degree of biological detail. 
CARLsim allows execution of networks of Izhikevich spiking neurons with realistic synaptic dynamics on both 
generic x86 CPUs and standard off-the-shelf GPUs. 
The simulator provides a PyNN-like programming interface in C/C++, 
which allows for details and parameters to be specified at the synapse, neuron, and network level.

New features in CARLsim 6 include:
- CUDA 11 support
- CMake build system
- Neuromodulatory features
- Integration of Python LEAP 
- Axonal Plasticity learning rule (release 6.1)

If you use CARLsim 6 in your research, please cite our [paper](https://ieeexplore.ieee.org/document/9892644)
Niedermeier, L., Chen, K., Xing, J., Das, A., Kopsick, J., Scott, E., Sutton, N., Weber, K., Dutt, N., and Krichmar, J.L. (2022).

"CARLsim 6: An Open Source Library for Large-Scale, Biologically Detailed Spiking Neural Network Simulation".  
Presented at the [WCCI2022 IEEE WORLD CONGRESS ON COMPUTATIONAL INTELLIGENCE, Padua, Italy, 2022](https://wcci2022.org/).  
In [Proceedings 2022 of IEEE International Joint Conference on Neural Networks (IJCNN)](https://ieeexplore.ieee.org/document/9892644).

For the new learning rule Axonal Plasticity, please cite this [paper](https://www.socsci.uci.edu/~jkrichma/CARLsim61-IJCNN2023.pdf):

Niedermeier, L. and Krichmar, J.L. (2023).  
"Experience-Dependent Axonal Plasticity in Large-Scale Spiking Neural Network Simulations".  
Presented at the [International Joint Conference on Neural Networks (IJCNN), Queensland, Australia, 2023](https://2023.ijcnn.org).   
To appear in  the [Proceedings 2023 of IEEE International Joint Conference on Neural Networks (IJCNN)](https://ieeexplore.ieee.org/xpl/conhome/1000500/all-proceedings).

## Quickstart for Linux

Detailed instructions for installing the latest stable release of CARLsim on Linux and Windows
can be found in our [User Guide](http://uci-carl.github.io/CARLsim6/ch1_getting_started.html).


### Build and Install


```
cd ~/git
git clone https://github.com/UCI-CARL/CARLsim6.git CARLsim6
cd CARLsim6
mkdir .build
cd .build
cmake -DCMAKE_INSTALL_PREFIX=/home/user1/carlsim6 -DCMAKE_BUILD_TYPE=Release ../.
make install
```


### Run simple SNN simulation

Open a new terminal and validate the settings with `env`.

Start `~/carlsim6/samples/hello_world`


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

If the Prerequisites cannot be met consider using a former version like CARLsim 5 or CARLsim 4.

The latest release was tested on the following platforms:  
- Linux:  Ubuntu 20.04 LTS  
- Windows: Windows 10 Professional, Windows 11 Education/Professional  
- CUDA: 11.2, 11.4, 11.5, 11.7  
- GPUs: Titan Xp, 1080ti, RTX 3090, A100  
