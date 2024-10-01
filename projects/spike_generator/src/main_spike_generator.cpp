/* * Copyright (c) 2016 Regents of the University of California. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. The names of its contributors may not be used to endorse or promote
*    products derived from this software without specific prior written
*    permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* *********************************************************************************************** *
* CARLsim
* created by: (MDR) Micah Richert, (JN) Jayram M. Nageswaran
* maintained by:
* (MA) Mike Avery <averym@uci.edu>
* (MB) Michael Beyeler <mbeyeler@uci.edu>,
* (KDC) Kristofor Carlson <kdcarlso@uci.edu>
* (TSC) Ting-Shuo Chou <tingshuc@uci.edu>
* (HK) Hirak J Kashyap <kashyaph@uci.edu>
*
* CARLsim v1.0: JM, MDR
* CARLsim v2.0/v2.1/v2.2: JM, MDR, MA, MB, KDC
* CARLsim3: MB, KDC, TSC
* CARLsim4: TSC, HK
* CARLsim5: HK, JX, KC
* CARLsim6: LN, JX, KC, KW
*
* CARLsim available from http://socsci.uci.edu/~jkrichma/CARLsim/
* Ver 12/31/2016
*/

// include CARLsim user interface
#include <carlsim.h>

// include stopwatch for timing
#include <stopwatch.h>
#include <stdlib.h>     // srand, rand
#include <iostream>     // std::cout

// custom ConnectionGenerator
class MyConnection : public ConnectionGenerator {
public:
    MyConnection() {}
    ~MyConnection() {}
    // the pure virtual function inherited from base class
    // note that weight, maxWt, delay, and connected are passed by reference
    void connect(CARLsim* sim, int srcGrp, int i, int destGrp, int j, float& weight, float& maxWt,
            float& delay, bool& connected) {
        
        //connected = (i==j) && (rand()/RAND_MAX < 0.1f);// connect n-th neuron in pre to n-th neuron in post (with 10% prob)
        connected = (i==j) && (rand()/RAND_MAX < 1.0f);// connect n-th neuron in pre to n-th neuron in post (with 10% prob)
        weight = 35.0f;//1.0
        //maxWt = 8.0f;//have to use maxWt?
        delay = 1.0;
    }
};

int main() {
	// keep track of execution time
	//Stopwatch watch;

	// ---------------- CONFIG STATE -------------------

// create a network on GPU
	int randSeed = 42; // random connection, change randseed number to generate different connection matrix with the same prob
	int numGPUs = 1;
#ifdef __NO_CUDA__
	CARLsim sim("Poisson_single_neuron", CPU_MODE, USER, numGPUs, randSeed);
#else
	CARLsim sim("Poisson_single_neuron", GPU_MODE, USER, numGPUs, randSeed);
#endif	//CARLsim sim("Poisson_single_neuron", GPU_MODE, DEVELOPER, numGPUs, randSeed);


    int N = 100;
#ifdef __NO_CUDA__
	int gin = sim.createSpikeGeneratorGroup("gin", N, EXCITATORY_NEURON, 0, CPU_CORES);
	int gout1 = sim.createGroup("gout1", 10, EXCITATORY_NEURON, 0, CPU_CORES);
	int gout2 = sim.createGroup("gout2", 3, INHIBITORY_NEURON, 0, CPU_CORES);
#else
	int gin = sim.createSpikeGeneratorGroup("gin", N, EXCITATORY_NEURON, 0, GPU_CORES);     
	int gout1 = sim.createGroup("gout1", 10, EXCITATORY_NEURON, 0, GPU_CORES); 
	int gout2 = sim.createGroup("gout2", 3, INHIBITORY_NEURON, 0, GPU_CORES); 
#endif

// set-up network parameters(C,k,Vr,Vt,a,b,Vpeak,Vmin,d)                     
    sim.setNeuronParameters(gout1, 100.0f, 0.0f, 0.7f, 0.0f, -60.0f, 0.0f, -40.0f, 0.0f, 0.03f, 0.0f, -2.0f, 0.0f, 35.0f, 0.0f, -50.0f, 0.0f, 100.0f, 0.0f);//regular spiking 9 parameters

    sim.setNeuronParameters(gout2, 205.0, 0.0, 0.52, 0.0, -64.64, 0.0, -56.25, 0.0, 0.002, 0.0, 42.24, 0.0, 0.17, 0.0, -60.62, 0.0, -19.0, 0.0);//CA1 Tri, subtype2,ok
    
    //sim.setSTP(gout1, true, 0.45f, 50.0f, 750.0f);
                                     
    //sim.setSTP(gout2, true, 0.15f, 750.0f, 50.0f);
                                     
    
    MyConnection* myConn = new MyConnection;
    //short int gin_gout1 = sim.connect(gin, gout1, myConn, 1.0, 0.5,SYN_FIXED);
    //short int gout2_gout1 = sim.connect(gout2, gout1, myConn, 1.0, 0.5,SYN_FIXED);
    
    short int gin_gout1 = sim.connect(gin, gout1, myConn, 1.0, 0.5,SYN_FIXED);//excitatory connection
    short int gout2_gout1 = sim.connect(gout2, gout1, myConn, 1.0, 0.5,SYN_FIXED);//inhibitory connection
                        

    float tdAMPA = 17.5;
    float tdNMDA = 50;
    float tdGABAa = 4;
    float tdGABAb = 150;
    sim.setConductances(true, tdAMPA, tdNMDA, tdGABAa, tdGABAb);

    //set neuronmonitor
    sim.setNeuronMonitor(gin,"DEFAULT");                          
    sim.setNeuronMonitor(gout1,"DEFAULT");
    sim.setNeuronMonitor(gout2,"DEFAULT");

	sim.setIntegrationMethod(RUNGE_KUTTA4, 10); //don't change! 5
	

	// ---------------- SETUP STATE -------------------
	// build the network
	//watch.lap("setupNetwork");

    // load simulation before setupNetwork
	//FILE* simFid = NULL;
    //simFid = fopen("SingeNeuronSNN.dat", "rb");
    
    //if (simFid == NULL) {
    //perror("Error opening file");
    //return 1;
//}
    //sim.loadSimulation(simFid);

    sim.setupNetwork();
    
    // ... wait until after setupNetwork is called
    //fclose(simFid);
        
    // include header file that contains generation of groups and their properties

#ifdef __NO_CUDA__
	PoissonRate poissRate(N, false);                   // allocate on GPU for minimal memory copies
#else
	PoissonRate poissRate(N, true);                   // allocate on GPU for minimal memory copies
#endif

    poissRate.setRates(0.69f);                            // set all rates to 50 Hz
    sim.setSpikeRate(gin, &poissRate);

    sim.setSpikeMonitor(gin, "DEFAULT"); 
    sim.setSpikeMonitor(gout1, "DEFAULT");
    sim.setSpikeMonitor(gout2, "DEFAULT");
	
	// save simulation
	//sim.saveSimulation("SingeNeuronSNN.dat", true); // fileName, saveSynapseInfo
	


// ---------------- RUN STATE -------------------
float scale = 1.0f;
for (int i=0; i<=4; i++) { // (int i=0; i<=53; i++)
        if (i == 0)
        {  
        sim.runNetwork(5,0);
        }
        
        if (i > 0 && i <= 4)
			{
			sim.scaleWeights(gin_gout1, scale, true); //scale excitatory
			std::cout << "scale excitatory connection with scale: " << scale << std::endl;
			
		    sim.scaleWeights(gout2_gout1 , scale, true);// scale inhibitory connection
		    std::cout << "scale inhibitory connection with scale: " << scale << std::endl;
		    
		    sim.runNetwork(5,0);
			}
	}
    
}


    

    

