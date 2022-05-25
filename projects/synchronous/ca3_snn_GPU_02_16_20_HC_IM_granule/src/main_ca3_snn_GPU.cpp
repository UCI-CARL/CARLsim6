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
*
* CARLsim available from http://socsci.uci.edu/~jkrichma/CARLsim/
* Ver 12/31/2016
*/

// include CARLsim user interface
#include <carlsim.h>

// Include libraries that will allow for us to perform vector operations, and
// print their results
#include <iostream>
#include <algorithm>
#include <vector>
#include <boost/iterator/counting_iterator.hpp>
#include <ctime>
#include <cstdlib>

// include stopwatch for timing
#include <stopwatch.h>



int main() 
{
	// keep track of execution time
	Stopwatch watch;

	// ---------------- CONFIG STATE -------------------

	// boost => getarg !!!
	const int arg1 = 1;
	float scale_granule;	// e.g 0.001f * 394502 = 394
	float scale_pyramidal;  // e.g. 0.01f * 74466 = 744

	switch (arg1) {  
	case 0: // dev
		scale_granule = 0.001f;    // 394
		scale_pyramidal = 0.01f;   // 744
		break;
	case 1: // 24G
		scale_granule = 0.01f;    // 3945
		scale_pyramidal = 1.0f;   // 74466
		break;
	default:
		return -1;
	}



	// create a network on GPU
	int numGPUs = 0;
	int randSeed = 10;
	CARLsim sim("ca3_snn_GPU", GPU_MODE, USER, numGPUs, randSeed);

	// include header file that contains generation of groups and their
	// properties
	#include "../generateCONFIGStateSTP.h"


	// -----------------------------------------------------------
	// Input Dentate Gyros
	
	 
	int DG_Granule = sim.createSpikeGeneratorGroup("DG__Granule", 394502 * scale_granule,
		EXCITATORY_NEURON, 0, GPU_CORES
	);

	// Connections 
	// Table 3 Connection probability	
	// Table 4 Synaptic delays
	
	sim.connect(DG_Granule, CA3_Pyramidal, "random", RangeWeight(0.0f, 1.45f, 2.0f), std::min<float>(0.02f, 0.02f / scale_granule ),
		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 1.262911855f, 0.0f
	);

	sim.connect(DG_Granule, CA3_Axo_Axonic, "random", RangeWeight(0.0f, 0.7f, 1.7f), std::min<float>(0.01f, 0.01f / scale_granule ),
		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 0.932621138f, 0.0f);

	sim.connect(DG_Granule, CA3_Basket, "random", RangeWeight(0.0f, 0.65f, 2.0f), std::min<float>(0.01f, 0.01f / scale_granule ),
		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 1.4977493f, 0.0f
	);

	sim.connect(DG_Granule, CA3_Bistratified, "random", RangeWeight(0.0f, 0.7f, 1.7f), std::min<float>(0.01f, 0.01f / scale_granule ),
		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 0.883682094f, 0.0f);

	sim.connect(DG_Granule, CA3_BC_CCK, "random", RangeWeight(0.0f, 1.0f, 2.0f), std::min<float>(0.01f, 0.01f / scale_granule ),
 		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 0.847532877f, 0.0f);

	sim.connect(DG_Granule, CA3_Ivy, "random", RangeWeight(0.0f, 1.35f, 2.35f), std::min<float>(0.01f, 0.01f / scale_granule ),
		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 1.314331038f, 0.0f);

	sim.connect(DG_Granule, CA3_MFA_ORDEN, "random", RangeWeight(0.0f, 0.75f, 2.0f), std::min<float>(0.01f, 0.01f / scale_granule ),
		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 1.35876774f, 0.0f
	);

	sim.connect(DG_Granule, CA3_QuadD_LM, "random", RangeWeight(0.0f, 1.25f, 2.25f), std::min<float>(0.01f, 0.01f / scale_granule ),
		RangeDelay(1, 10), RadiusRF(-1.0), SYN_PLASTIC, 0.874424964f, 0.0f);


	// 
	// Table 4 STP parameters
	
	sim.setSTP(DG_Granule, CA3_Pyramidal, true, STPu(0.16f, 0.0f),
		STPtauU(42.00f, 0.0f),
		STPtauX(347.44f, 0.0f),
		STPtdAMPA(7.43f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);

	sim.setSTP(DG_Granule, CA3_Axo_Axonic, true, STPu(0.17f, 0.0f),
		STPtauU(36.83f, 0.0f),
		STPtauX(702.40f, 0.0f),
		STPtdAMPA(4.13f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);

	sim.setSTP(DG_Granule, CA3_Basket, true, STPu(0.19f, 0.0f),
		STPtauU(30.29f, 0.0f),
		STPtauX(744.66f, 0.0f),
		STPtdAMPA(3.58f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);

	sim.setSTP(DG_Granule, CA3_BC_CCK, true, STPu(0.21f, 0.0f),
		STPtauU(25.92f, 0.0f),
		STPtauX(658.70f, 0.0f),
		STPtdAMPA(3.46f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);

	sim.setSTP(DG_Granule, CA3_Bistratified, true, STPu(0.19f, 0.0f),
		STPtauU(38.34f, 0.0f),
		STPtauX(594.72f, 0.0f),
		STPtdAMPA(4.39f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);

	sim.setSTP(DG_Granule, CA3_Ivy, true, STPu(0.18f, 0.0f),
		STPtauU(21.48f, 0.0f),
		STPtauX(764.65f, 0.0f),
		STPtdAMPA(3.75f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);

	sim.setSTP(DG_Granule, CA3_MFA_ORDEN, true, STPu(0.19f, 0.0f),
		STPtauU(48.65f, 0.0f),
		STPtauX(453.65f, 0.0f),
		STPtdAMPA(4.87f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);

	sim.setSTP(DG_Granule, CA3_QuadD_LM, true, STPu(0.18f, 0.0f),
		STPtauU(43.06f, 0.0f),
		STPtauX(464.59f, 0.0f),
		STPtdAMPA(4.96f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)
	);


	sim.setNeuronMonitor(DG_Granule, "DEFAULT");



	// Set the time constants for the excitatory and inhibitory receptors, and
	// set the method of integration to numerically solve the systems of ODEs
	// involved in the SNN
	// sim.setConductances(true);
	sim.setIntegrationMethod(RUNGE_KUTTA4, 5);

	// ---------------- SETUP STATE -------------------
	// build the network
	watch.lap("setupNetwork");
	sim.setupNetwork();

	// include header file that contains generation of groups and their
	// properties
	#include "../generateSETUPStateSTP.h"

	sim.setSpikeMonitor(DG_Granule, "DEFAULT");

	// Set the seed of the pseudo-random number generator based on the current system time
	//std::srand(std::time(nullptr));
	std::srand(42);
	int DG_Granule_frate = 100.0f;

	PoissonRate DG_Granule_rate(394502 * scale_granule, true); // create PoissonRate object for all Granule cells
	//DG_Granule_rate.setRates(0.4f); // set all mean firing rates for the object to 0.4 Hz
	DG_Granule_rate.setRates(DG_Granule_frate); // set all mean firing rates for the object to 0.4 Hz
	sim.setSpikeRate(DG_Granule, &DG_Granule_rate, 1); // link the object with defined Granule cell group, with refractory period 1 ms


	// Declare variables that will store the start and end ID for the neurons
	// in the granule group
	int DG_start = sim.getGroupStartNeuronId(DG_Granule);
	int DG_end = sim.getGroupEndNeuronId(DG_Granule);
	int DG_range = (DG_end - DG_start) + 1;

	// Create a vector that is the length of the number of neurons in the granule population
	std::vector<int> DG_vec(boost::counting_iterator<int>(0),
		boost::counting_iterator<int>(DG_range));

	// Define the number of granule cells to fire
	int numGranuleFire = 100;

	std::vector<int> DG_vec_A;

	// Define the location of those granule cells so that we choose the same granule cells each time we call setRates
	for (int i = 0; i < numGranuleFire; i++)
	{
		//DG_vec_A.push_back(5 * (i + 1));
		DG_vec_A.push_back(2*(i + 1));
	}


	// ---------------- RUN STATE -------------------
	watch.lap("runNetwork");

	// run for a total of 9 seconds
	// at the end of each runNetwork call, SpikeMonitor stats will be printed
	for (int i = 0; i <= 18; i++)
	//for (int i = 0; i < 4; i++)
	{
		sim.runNetwork(0, 500); // run network for 500 ms
/*		
		if (i >= 0 && i < 10)
		{
			sim.runNetwork(0, 500); // run network for 500 ms
		}

		if (i == 10)
		{
			for (int j = 0; j < numGranuleFire; j++)
			{
				int randGranCell = DG_vec.front() + DG_vec_A[j]; // choose the jth random granule cell
				DG_Granule_rate.setRate(DG_vec.at(randGranCell), DG_Granule_frate); // set the firing rate for the jth random granule cell
			}
			sim.setSpikeRate(DG_Granule, &DG_Granule_rate, 1); // update the firing rates of all granule cells before the next run of the simulation           
			sim.runNetwork(0, 25); // run network for 25 ms
		}

		if (i == 11)
		{
			DG_Granule_rate.setRates(0.4f); // set the firing rates for all granule cells back to baseline firing rate
			sim.setSpikeRate(DG_Granule, &DG_Granule_rate, 1); // update the firing rates of all granule cells before the next run of the simulation          
			sim.runNetwork(0, 75); // run network for 75 ms
		}

		if (i == 12)
		{
			for (int j = 0; j < numGranuleFire; j++)
			{
				int randGranCell = DG_vec.front() + DG_vec_A[j]; // choose the jth random granule cell
				DG_Granule_rate.setRate(DG_vec.at(randGranCell), DG_Granule_frate); // set the firing rate for the jth random granule cell
			}
			sim.setSpikeRate(DG_Granule, &DG_Granule_rate, 1); // update the firing rates of all granule cells before the next run of the simulation           
			sim.runNetwork(0, 25); // run network for 25 ms
		}

		if (i == 13)
		{
			DG_Granule_rate.setRates(0.4f); // set the firing rates for all granule cells back to baseline firing rate
			sim.setSpikeRate(DG_Granule, &DG_Granule_rate, 1); // update the firing rates of all granule cells before the next run of the simulation
			sim.runNetwork(0, 75); // run network for 75 ms
		}

		if (i >= 14 && i < 22)
		{
			sim.runNetwork(0, 500); // run network for 500 ms
		}

		if (i >= 14 && i == 22)
		{
			sim.runNetwork(0, 437); // run network for the remaining time
		}
*/
	}

	// print stopwatch summary
	watch.stop();

	return 0;
}
