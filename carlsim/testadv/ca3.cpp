
/*


*/

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>


#include <periodic_spikegen.h>
//#include <stopwatch.h>


//! test the effect of short-term depression - Using single GPU/CPU
/*!
 * \brief validate CSTP by pyramidal cells without inhibitory inter neurons  
 *
 */
TEST(CA3, CSTP_PC) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	// keep track of execution time
	//Stopwatch watch;

	// ---------------- CONFIG STATE -------------------

	// create a network on GPU
	int numGPUs = 0;
	int randSeed = 10;

	//CARLsim * sim = new CARLsim("ca3_snn_GPU", GPU_MODE, USER, numGPUs, randSeed);
	//const ComputingBackend BACKEND_CORES = GPU_CORES;

	CARLsim* sim = new CARLsim("ca3__cstp_pc", CPU_MODE, USER, numGPUs, randSeed);
	const ComputingBackend BACKEND_CORES = CPU_CORES;

	// include header file that contains generation of groups and their
	// properties
	//#include "../generateCONFIGStateSTP.h"
	int CA3_Pyramidal = sim->createGroup("CA3_Pyramidal", 74,  
		EXCITATORY_NEURON, 0, BACKEND_CORES);

	sim->setNeuronParameters(CA3_Pyramidal, 366.0, 0.0, 0.792338703789581, 0,
		-63.2044008171655, 0.0, -33.6041733124267, 0.0, 0.00838350334098279,
		0.0, -42.5524776883928, 0.0, 35.8614648558726,
		0.0, -38.8680990294091, 0.0,
		588.0, 0.0, 1);

	// CS6, Windows
	sim->connect(CA3_Pyramidal, CA3_Pyramidal, "random", RangeWeight(0.0f, 0.55f, 1.55f), 0.75, // 0.0250664662231983f,   
		RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);


	//sim->connect(CA3_Pyramidal, CA3_Pyramidal, "random", RangeWeight(0.0f, 0.55f, 1.55f), 0.75, // 0.0250664662231983f,   
	//	RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.1f);  // no more activity

	//sim->connect(CA3_Pyramidal, CA3_Pyramidal, "random", RangeWeight(0.55f), 0.5, // 0.0250664662231983f,   
	//	RangeDelay(1, 2), RadiusRF(-1.0), SYN_FIXED, 0.553062478f, 0.0f);


	//sim->setSTP(CA3_Pyramidal, CA3_Pyramidal, true, 
	//	STPu(0.27922089865f, 0.0f),
	//	STPtauU(21.44820657f, 0.0f),
	//	STPtauX(318.510891f, 0.0f),
	//	STPtdAMPA(10.21893984f, 0.0f),
	//	STPtdNMDA(150.0f, 0.0f),
	//	STPtdGABAa(6.0f, 0.0f),
	//	STPtdGABAb(150.0f, 0.0f),
	//	STPtrNMDA(0.0f, 0.0f),
	//	STPtrGABAb(0.0f, 0.0f));

	//// undef
	//sim->setSTP(CA3_Pyramidal, CA3_Pyramidal, true,
	//	STPu(0.45f), 
	//	STPtauU(50.0f), 
	//	STPtauX(750.0f),
	//	STPtdAMPA(1.0f, 0.0f),
	//	STPtdNMDA(15.0f, 0.0f),
	//	STPtdGABAa(600.0f, 0.0f),
	//	STPtdGABAb(15.0f, 0.0f),
	//	STPtrNMDA(0.0f, 0.0f),
	//	STPtrGABAb(0.0f, 0.0f)); 

	// knocks of after few 100ms 
	sim->setSTP(CA3_Pyramidal, CA3_Pyramidal, true,
		STPu(0.15f),
		STPtauU(750.0f),  // 750
		STPtauX(50.0f),   // 50
		//STPtdAMPA(0.5f, 0.0f),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
		STPtdAMPA(0.5, 0.0),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
		STPtdNMDA(0.1f, 0.0f),
		STPtdGABAa(0.1f, 0.0f),
		STPtdGABAb(0.1f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f)); 


	// reproduce access violation by setting setSTP to false
	// Exception thrown at 0x00007FFF4BEDB87E (carlsimd.dll) in carlsim - tests.exe: 0xC0000005 : Access violation reading location 0x0000000000000000.
	// .\kernel\src\snn_cpu_module.cpp
	// 660 					runtimeData[netId].stpu[ind_plus] = runtimeData[netId].stpu[ind_minus] * (1.0f - runtimeData[netId].stp_tau_u_inv[lSId]);
	//
	//sim->setSTP(CA3_Pyramidal, CA3_Pyramidal, false, STPu(0.27922089865f, 0.0f),
	//	STPtauU(21.44820657f, 0.0f),
	//	STPtauX(318.510891f, 0.0f),
	//	STPtdAMPA(10.21893984f, 0.0f),
	//	STPtdNMDA(150.0f, 0.0f),
	//	STPtdGABAa(6.0f, 0.0f),
	//	STPtdGABAb(150.0f, 0.0f),
	//	STPtrNMDA(0.0f, 0.0f),
	//	STPtrGABAb(0.0f, 0.0f));

	
	NeuronMonitor* nrnMon_Pyramidal = sim->setNeuronMonitor(CA3_Pyramidal, "DEFAULT");


	// Set the time constants for the excitatory and inhibitory receptors, and
	// set the method of integration to numerically solve the systems of ODEs
	// involved in the SNN
	// sim->setConductances(true);
	sim->setIntegrationMethod(RUNGE_KUTTA4, 5);

	// ---------------- SETUP STATE -------------------
	// build the network
	//watch.lap("setupNetwork");
	sim->setupNetwork();

	//ConnectionMonitor* connMon_PC_PC = sim->setConnectionMonitor(CA3_Pyramidal, CA3_Pyramidal, "DEFAULT");
	//connMon_PC_PC->setUpdateTimeIntervalSec(-1);


	// Declare variables that will store the start and end ID for the neurons
	// in the pyramidal group
	int pyr_start = sim->getGroupStartNeuronId(CA3_Pyramidal);
	std::cout << "Beginning neuron ID for Pyramidal Cells is : " << pyr_start << "\n";
	int pyr_end = sim->getGroupEndNeuronId(CA3_Pyramidal);
	std::cout << "Ending neuron ID for Pyramidal Cells is : " << pyr_end << "\n";
	int pyr_range = (pyr_end - pyr_start) + 1;
	std::cout << "The range for Pyramidal Cells is : " << pyr_range << "\n";


	// include header file that contains generation of groups and their
	// properties
	//#include "../generateSETUPStateSTP.h"
	sim->setSpikeMonitor(CA3_Pyramidal, "DEFAULT");

	// ---------------- RUN STATE -------------------

	nrnMon_Pyramidal->startRecording();

	// run for a total of 10 seconds in 500ms bins
	// at the end of each runNetwork call, SpikeMonitor stats will be printed
	for (int i = 0; i < 3 * 2; i++) {
		if (i == 0)
		{
			// run 10ms with stimulus 
			//watch.lap("runNetwork(init)");
			//connMon_PC_PC->takeSnapshot();

			// simulated input from dentate gyros (DG)
			sim->setExternalCurrent(CA3_Pyramidal, 1000.0f);  // 500 -> 300ms,  1000 => 50ms

			//// alternative input from DG
			//sim->setExternalCurrent(CA3_Pyramidal, 40.0f);

			sim->runNetwork(0, 10, true);
			printf("\n");

			// reset stimulus 
			sim->setExternalCurrent(CA3_Pyramidal, 0.0f);

			sim->runNetwork(0, 10, true);
			printf("\n");

			sim->runNetwork(0, 30, true);
			printf("\n");

			sim->runNetwork(0, 50, true);
			printf("\n");

			for (int j = 0; j < 4; j++) {
				sim->runNetwork(0, 100, true);
				printf("\n");
			}

			//watch.lap("runNetwork");
		}
		else
			if (i > 0)
			{
				sim->runNetwork(0, 500, true);
				printf("\n");
			}

	}

	//sim->runNetwork(90, 0, true);

	nrnMon_Pyramidal->stopRecording();
	//nrnMon_Pyramidal->print(true);

	//connMon_PC_PC->takeSnapshot();

	// print stopwatch summary
	//watch.stop();

	delete sim;

	printf("done.\n");
}
