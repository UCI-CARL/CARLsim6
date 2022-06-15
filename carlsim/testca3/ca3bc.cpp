
/*
Neuron Types of synchronous basemodel

Concept: Apply aribtrary Markov process
E_i = (t_i, f_pois, I_c)  -->  (f, ..)_t   t = 1,...  sim_type,  
compare t_i when change happens -> describe as events E_i
*/

#include "gtest/gtest.h"
#include "carlsim_tests.h"
#include <carlsim.h>
#include <periodic_spikegen.h>


//! test the effect of short-term depression - Using single GPU/CPU
/*!
 * \brief Measures neural activity of CA3 BC due excitability by constant input I_c or a Poisson generator 
 *
 */
TEST(CA3, excit_BC) {   
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

#include "ca3types.h"

	// CARLsim parameter
	LoggerMode logger = SILENT;  // USER, SILENT
	int numGPUs = 0;
	int randSeed = 42;

	enum RUN { POISSON, CURRENT };   // distinct inputs as poison generator and I_c array
	const char* run_names[] = { "POISSON", "CURRENT" };

	struct RESULT {
		RUN run;
		int mode; 
		int af;  // Average Spiking Frequency
		int spikes;
	};

	const auto len = sizeof(RUN);
	RESULT results[TESTED_MODES][len];  // check if len or bytes

	for (int mode = 0; mode < TESTED_MODES; mode++) {	 // start with CPU for dev/unit tests
	//for (int mode = TESTED_MODES - 1; mode >= 0;  mode--) {  // start with GPU for perf tests
	//{int mode = 0; // run single backend, 0 for CPU, 1 for GPU

		for (int run = RUN(POISSON); run <= RUN(CURRENT); run++) {
		//{int run = RUN(POISSON);
		//{int run = RUN(CURRENT); 
			// keep references local
			int neuronGroup[n_NT];  // todo init with -1  -> std:: array ... 
			//int connGroup[n_NT][n_NT]; // pre, post

			CARLsim* sim = new CARLsim(mode ? "ca3_bc_GPU" : "ca3_bc_CPU", mode ? GPU_MODE : CPU_MODE, logger, numGPUs, randSeed);
			BACKEND_CORES = mode ? GPU_CORES : CPU_CORES;

			// ---------------- CONFIG STATE -------------------
			sim->setIntegrationMethod(RUNGE_KUTTA4, 5);

			// amount of neurons
			//int n = 1;    // developement
			int n = 10;   // unit test
			//int n = 100;	// functional tests
			//int n = 400;	// functional tests
			//int n = 743;  // example
			//int n = 74366;	// CA3 mouse

			create_BC(sim, neuronGroup, n);
			EXPECT_GE(neuronGroup[BC], 0); 

			int g0; // input group
			float wt; // weight 
			nullptr;

			switch (run) {
			case RUN(POISSON): {
				// poisson source
				float rate = 0.85f; // Hz
				g0 = sim->createSpikeGeneratorGroup("input0", n, EXCITATORY_NEURON, -1, BACKEND_CORES);
				bool spikeAtZero = true;
				PeriodicSpikeGenerator* spkGenG0 = new PeriodicSpikeGenerator(rate, spikeAtZero); // periodic spiking @ <rate> Hz
				sim->setSpikeGenerator(g0, spkGenG0);

				// connect input to target
				wt = 37.0f; // COBA

				sim->connect(g0, neuronGroup[BC], "one-to-one", RangeWeight(0.0f, wt, 40.0f), 1.0, // 0.0250664662231983f,   
					RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);
				//RangeDelay(1), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);

				sim->setSTP(g0, neuronGroup[BC], true,
					STPu(0.65f),
					STPtauU(75.0f),  // 750
					STPtauX(750.0f),   // 50
					STPtdAMPA(1.25 * 15.0f, 0.0f),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
					STPtdNMDA(150.0f, 0.0f),
					STPtdGABAa(6.0f, 0.0f),
					STPtdGABAb(150.0f, 0.0f),
					STPtrNMDA(0.0f, 0.0f),
					STPtrGABAb(0.0f, 0.0f));
				break;

			}							 
			case RUN(CURRENT): {
				g0 = sim->createGroup("input0", n, EXCITATORY_NEURON, -1, BACKEND_CORES);
				sim->setNeuronParameters(g0, 366.0, 0.0, 0.792338703789581, 0,
					-63.2044008171655, 0.0, -33.6041733124267, 0.0, 0.00838350334098279,
					0.0, -42.5524776883928, 0.0, 35.8614648558726,
					0.0, -38.8680990294091, 0.0,
					588.0, 0.0, 1);

				// connect input to target
				wt = 40.0f; // COBA  post syn group starts firing at 1700ms

				sim->connect(g0, neuronGroup[BC], "one-to-one", RangeWeight(0.0f, wt, 40.0f), 0.75, // 0.0250664662231983f,   
					RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);
				sim->setSTP(g0, neuronGroup[BC], true,
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
			}
			default:
				;
			}
	
			// ---------------- SETUP STATE -------------------
			// build the network
			sim->setupNetwork();

			switch (run) {
			case RUN(POISSON):
				break;
			case RUN(CURRENT):
				sim->setExternalCurrent(g0, 0.2f);  // 1100ms: 10,10,20 Hz
				break;
			default:
				;
			}

			//SpikeMonitor* spkMon0 = sim->setSpikeMonitor(g0, "DEFAULT");
			SpikeMonitor* spkMon1 = sim->setSpikeMonitor(neuronGroup[BC], "DEFAULT");

			// ---------------- RUN STATE -------------------
			//nrnMon_Pyramidal1->startRecording();

			// run for a total of 10 seconds in 500ms bins
			// at the end of each runNetwork call, SpikeMonitor stats will be printed

			results[mode][run].mode = mode;
			results[mode][run].run = RUN(run);
			results[mode][run].spikes = -1;
			results[mode][run].af = -1;

			try {
				for (int i = 0; i < 2 * 2; i++) {
					if (i == 0)
					{
						// run 10ms with stimulus 
						// 
						//// alternative input from DG
						//sim->setExternalCurrent(neuronGroup[PC], 10.0f);

						sim->runNetwork(0, 10, true);						
						//// reset stimulus 
						////sim->setExternalCurrent(CA3_Pyramidal1, 0.0f);

						sim->runNetwork(0, 10, true);
						sim->runNetwork(0, 30, true);
						sim->runNetwork(0, 50, true);
						for (int j = 0; j < 4; j++) {
							sim->runNetwork(0, 100, true);
						}
					}
					else
						if (i > 0)
						{
							for (int j = 0; j < 5; j++) {
								spkMon1->startRecording();
								sim->runNetwork(0, 100, true);
								spkMon1->stopRecording();
								int t = sim->getSimTime();
								float af = spkMon1->getPopMeanFiringRate();
								if (af > 7.5f) {  // Hz
									//printf("%5d ms %s %s MeanFiringRate(PC) %.1f Hz\n", t, mode == GPU_MODE ? "GPU" : "CPU", run_names[run], af);
									results[mode][run].spikes = t;
									results[mode][run].af = af;
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
			delete sim;
		}

		// expected value E and eps
		EXPECT_LE(std::abs(results[mode][POISSON].spikes - 1200), 200);	// 1200 ms
		EXPECT_LE(std::abs(results[mode][CURRENT].spikes - 1200), 200); // 1200 ms
		EXPECT_LE(std::abs(results[mode][POISSON].af - 30), 15);		// 20-40 Hz
		EXPECT_LE(std::abs(results[mode][CURRENT].af - 30), 15);		// 20-40 Hz
	}

	// compare GPU vs CPU by eps
	EXPECT_LE(std::abs(results[GPU_MODE][POISSON].spikes - results[CPU_MODE][POISSON].spikes), 200); // ms
	EXPECT_LE(std::abs(results[GPU_MODE][CURRENT].spikes - results[CPU_MODE][CURRENT].spikes), 200); // ms
}
