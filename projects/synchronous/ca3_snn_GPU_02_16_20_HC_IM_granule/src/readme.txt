
The DG CA3 model was tuned to run with 24GB device memory, while 
generating the gamma oscillation as described in [22], Fig. 7.


    |-+ Local Connection Id 58: DG__Granule(8) => CA3_QuadD_LM(0)
      |- Type                       =  PLASTIC
      |- Min weight                 =  0.00000
      |- Max weight                 =  2.25000
      |- Initial weight             =  1.25000
      |- Min delay                  =        1
      |- Max delay                  =       10
      |- Radius X                   =    -1.00
      |- Radius Y                   =    -1.00
      |- Radius Z                   =    -1.00
      |- Num of synapses            = 129599
      |- Avg numPreSynapses         =    39.51
      |- Avg numPostSynapses        =    32.85
  
  *****************      Initializing GPU 0 Runtime      ***********************
  GPU Memory Management: (Total 24258.938 MB)
  Data			Size		Total Used	Total Available
  Init:			24258.938 MB	547.562 MB	23711.375 MB
  Random Gen:		6.000 MB	553.562 MB	23705.375 MB
  Static Load:		0.000 MB	553.562 MB	23705.375 MB
  Conn Info:		3920.000 MB	4473.562 MB	19785.375 MB
  Syn State:		13966.000 MB	18439.562 MB	5819.375 MB
  Neuron State:		3946.000 MB	22385.562 MB	1873.375 MB
  Group State:		0.000 MB	22385.562 MB	1873.375 MB
  Auxiliary Data:		1690.000 MB	24075.562 MB	183.375 MB
  
  
  SpikeMonitor set for group 0 (CA3_QuadD_LM)
  SpikeMonitor set for group 1 (CA3_Axo_Axonic)
  SpikeMonitor set for group 2 (CA3_Basket)
  SpikeMonitor set for group 3 (CA3_BC_CCK)
  SpikeMonitor set for group 4 (CA3_Bistratified)
  SpikeMonitor set for group 5 (CA3_Ivy)
  SpikeMonitor set for group 6 (CA3_MFA_ORDEN)
  SpikeMonitor set for group 7 (CA3_Pyramidal)
  SpikeMonitor set for group 8 (DG__Granule)
  
  ******************** Running the simulation on 1 GPU(s) and 0 CPU(s) *********



You can apply the following measure, when you are using a desktop e.g. Gnome:
1. Close all non essential apps, like Firefox 
2. Alternatively, reboot in runlevel 3

Use the tools provided with CUDA to check the current memory usage: 

+-----------------------------------------------------------------------------+
| NVIDIA-SMI 495.29.05    Driver Version: 495.29.05    CUDA Version: 11.5     |
|-------------------------------+----------------------+----------------------+
| GPU  Name        Persistence-M| Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp  Perf  Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |
|                               |                      |               MIG M. |
|===============================+======================+======================|
|   0  NVIDIA GeForce ...  On   | 00000000:01:00.0  On |                  N/A |
| 31%   64C    P2   239W / 350W |  24087MiB / 24258MiB |    100%      Default |
|                               |                      |                  N/A |
+-------------------------------+----------------------+----------------------+
                                                                               
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|    0   N/A  N/A      3888      G   /usr/lib/xorg/Xorg                 66MiB |
|    0   N/A  N/A      4266      G   /usr/lib/xorg/Xorg                143MiB |
|    0   N/A  N/A      4398      G   /usr/bin/gnome-shell               78MiB |
|    0   N/A  N/A      5800      C   ca3_snn_GPU_sync_granule        23781MiB |
+-----------------------------------------------------------------------------+

