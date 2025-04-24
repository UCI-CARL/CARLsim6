

# Lars (4/24/2025)


1. fixed tutoral CMake and file namings

2. see carlsim-spike-generators as templates.

3. see test/spikegen how to use them

4. implement a normal distributed spike genetator class. 
   use spikegen_normal_dist.h as starting point and API 
  
5. The normal RV needs to be discrete (int): 400 spikes sd 2.4ms (see paper) 
     #95 //Calculate the normal distribution value
    see Matlab doc\source\tutorial\10_synfire_network\scripts\NormalDistributedStimulusPackage.m
    and normal_dist_stim.png

6. write a test case in test/spikegen, apply the 3 synfire 
   asset 400 spikes and sd 2.4, 0.8, 3.2
   CAUTION: Be aware of 0 as RV versus 0 as index  

7. clone exc0 as spikegen group and connect 1:1 

  
 
