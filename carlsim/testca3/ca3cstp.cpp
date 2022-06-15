
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

	// CARLsim parameter
	LoggerMode logger = SILENT;  // USER, SILENT
	ComputingBackend BACKEND_CORES;
	int numGPUs = 0;
	int randSeed = 10;

	enum RUN { STF, STD, NONE };

	struct RESULT {
		RUN run;
		int mode; 
		int spikes;  // ms when target group starts spiking 
	};

	const auto len = sizeof(RUN);
	RESULT results[TESTED_MODES][len];  // check if len or bytes

	for (int mode = 0; mode < TESTED_MODES; mode++) {

		for (int run = RUN(STF); run <= RUN(NONE); run++) {

			CARLsim* sim = new CARLsim(mode ? "ca3_cstp_pc_GPU" : "ca3_cstp_pc_CPU", mode ? GPU_MODE : CPU_MODE, logger, numGPUs, randSeed);
			BACKEND_CORES = mode ? GPU_CORES : CPU_CORES;


			// keep track of execution time
			//Stopwatch watch;

			// ---------------- CONFIG STATE -------------------

			// no comment
			sim->setIntegrationMethod(RUNGE_KUTTA4, 5);

			// include header file that contains generation of groups and their
			// properties
			//#include "../generateCONFIGStateSTP.h"
			int CA3_Pyramidal1 = sim->createGroup("CA3_Pyramidal1", 74,
				EXCITATORY_NEURON, 0, BACKEND_CORES);
			int CA3_Pyramidal2 = sim->createGroup("CA3_Pyramidal2", 74,
				EXCITATORY_NEURON, 0, BACKEND_CORES);

			sim->setNeuronParameters(CA3_Pyramidal1, 366.0, 0.0, 0.792338703789581, 0,
				-63.2044008171655, 0.0, -33.6041733124267, 0.0, 0.00838350334098279,
				0.0, -42.5524776883928, 0.0, 35.8614648558726,
				0.0, -38.8680990294091, 0.0,
				588.0, 0.0, 1);

			sim->setNeuronParameters(CA3_Pyramidal2, 366.0, 0.0, 0.792338703789581, 0,
				-63.2044008171655, 0.0, -33.6041733124267, 0.0, 0.00838350334098279,
				0.0, -42.5524776883928, 0.0, 35.8614648558726,
				0.0, -38.8680990294091, 0.0,
				588.0, 0.0, 1);


			// CS6, Windows
			// CS4_HC does not support fixed synapses anymore
			//if (RUN(run) == NONE)
			//	sim->connect(CA3_Pyramidal1, CA3_Pyramidal2, "one-to-one", RangeWeight(0.55f), 0.75, // 0.0250664662231983f,   
			//		RangeDelay(1, 2), RadiusRF(-1.0), SYN_FIXED,	0.553062478f, 0.0f);
			//else
				sim->connect(CA3_Pyramidal1, CA3_Pyramidal2, "one-to-one", RangeWeight(0.0f, 0.55f, 1.55f), 0.75, // 0.0250664662231983f,   
					RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC,	0.553062478f, 0.0f);


			switch (RUN(run)) {
			case STF:
				//	facilitation  spikes by 600 CPU
				//	facilitation  spikes by 700 GPU
				sim->setSTP(CA3_Pyramidal1, CA3_Pyramidal2, true,
					STPu(0.65f),
					STPtauU(75.0f),  // 750
					STPtauX(750.0f),   // 50
					STPtdAMPA(15.0f, 0.0f),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
					//STPtdAMPA(0.55, 0.0),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
					STPtdNMDA(150.0f, 0.0f),
					STPtdGABAa(6.0f, 0.0f),
					STPtdGABAb(150.0f, 0.0f),
					STPtrNMDA(0.0f, 0.0f),
					STPtrGABAb(0.0f, 0.0f));
				break;
			case STD:
				// depression 1100 CPU
				// depression 1000 GPU
				sim->setSTP(CA3_Pyramidal1, CA3_Pyramidal2, true,
					STPu(0.15f),
					STPtauU(750.0f),  // 750
					STPtauX(50.0f),   // 50
					STPtdAMPA(0.5f, 0.0f),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
					//STPtdAMPA(0.55, 0.0),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
					STPtdNMDA(0.2f, 0.0f),
					STPtdGABAa(0.1f, 0.0f),
					STPtdGABAb(0.1f, 0.0f),
					STPtrNMDA(0.0f, 0.0f),
					STPtrGABAb(0.0f, 0.0f));
				break;
			default:
				;
			}

			NeuronMonitor* nrnMon_Pyramidal1 = sim->setNeuronMonitor(CA3_Pyramidal1, "DEFAULT");
			NeuronMonitor* nrnMon_Pyramidal2 = sim->setNeuronMonitor(CA3_Pyramidal2, "DEFAULT");

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

			// include header file that contains generation of groups and their
			// properties
			//#include "../generateSETUPStateSTP.h"
			SpikeMonitor* spkMon1 = sim->setSpikeMonitor(CA3_Pyramidal1, "DEFAULT");
			SpikeMonitor* spkMon2 = sim->setSpikeMonitor(CA3_Pyramidal2, "DEFAULT");

			// ---------------- RUN STATE -------------------

			//nrnMon_Pyramidal1->startRecording();
			//nrnMon_Pyramidal2->startRecording();

			// run for a total of 10 seconds in 500ms bins
			// at the end of each runNetwork call, SpikeMonitor stats will be printed

			results[mode][run].mode = mode;
			results[mode][run].run = RUN(run);
			results[mode][run].spikes = -1;

			try {
				for (int i = 0; i < 4 * 2; i++) {
					if (i == 0)
					{
						// run 10ms with stimulus 
						//watch.lap("runNetwork(init)");
						//connMon_PC_PC->takeSnapshot();

						//// simulated input from dentate gyros (DG)
						//sim->setExternalCurrent(CA3_Pyramidal1, 100.0f);  // 500 -> 300ms,  1000 => 50ms

						//// alternative input from DG
						sim->setExternalCurrent(CA3_Pyramidal1, 500.0f);

						sim->runNetwork(0, 10, true);
						
						// reset stimulus 
						//sim->setExternalCurrent(CA3_Pyramidal1, 0.0f);

						sim->runNetwork(0, 10, true);

						sim->runNetwork(0, 30, true);

						sim->runNetwork(0, 50, true);

						for (int j = 0; j < 4; j++) {
							sim->runNetwork(0, 100, true);
						}

						//watch.lap("runNetwork");
					}
					else
						if (i > 0)
						{
							for (int j = 0; j < 5; j++) {
								spkMon2->startRecording();
								sim->runNetwork(0, 100, true);
								spkMon2->stopRecording();
								//printf("spkMon2->getPopMeanFiringRate() %f\n", spkMon2->getPopMeanFiringRate());
								if (spkMon2->getPopMeanFiringRate() > 7.5) {  // Hz
									results[mode][run].spikes = sim->getSimTime();
									throw std::exception("done");
								}
							}
						}
				}
			}
			catch (const std::exception& e) {
				if(std::string(e.what()) != "done")
					throw e; 
			}
			

			//sim->runNetwork(90, 0, true);

			//nrnMon_Pyramidal1->stopRecording();
			//nrnMon_Pyramidal2->stopRecording();
			//nrnMon_Pyramidal->print(true);

			//connMon_PC_PC->takeSnapshot();

			// print stopwatch summary
			//watch.stop();

			delete sim;

			//printf("done.\n");
		}

		//EXPECT_EQ(results[mode][NONE].spikes, -1);	// no spikes should have been fired without STP, CS4 GPU OK, CS4 CPU code fires after 1600
		EXPECT_LT(results[mode][STF].spikes, results[mode][STD].spikes); 
	}

	// compare GPU vs CPU
	EXPECT_LE(std::abs(results[GPU_MODE][STF].spikes - results[CPU_MODE][STF].spikes), 100);	
	EXPECT_LE(std::abs(results[GPU_MODE][STD].spikes - results[CPU_MODE][STD].spikes), 100);
	//EXPECT_EQ(results[GPU_MODE][NONE].spikes, results[CPU_MODE][NONE].spikes); // no spikes should have been fired without STP, CS4 GPU OK, CS4 CPU code fires after 1600

}
