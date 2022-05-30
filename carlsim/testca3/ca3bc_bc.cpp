
/*
Neuron Types of synchronous basemodel

Concept: Apply arbitrary Markov process
E_i = (t_i, f_pois, I_c)  -->  (f, ..)_t   t = 1,...  sim_type,  
compare t_i when change happens -> describe as events E_i
*/

#include <deque>

#include "gtest/gtest.h"
#include "carlsim_tests.h"
#include <carlsim.h>
#include <periodic_spikegen.h>



//! test the effect of short-term depression - Using single GPU/CPU
/*!
 * \brief Measures neural activity of CA3 BC due excitability by constant input I_c or a Poisson generator 

 * Sizing 
 * Recurrent Connected => Only trigger an initial stimulus
 * => differnt structure, differnt Markov chain => seperate test case     
 */
TEST(CA3, excit_BC_BC) {   
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

#include "ca3types.h"

	// CARLsim parameter
	LoggerMode logger = SILENT;  // USER, SILENT
	int numGPUs = 0;
	int randSeed = 42;

	enum RUN { POISSON, CURRENT };   // distinct inputs as poison generator and I_c array
	const char* run_names[] = { "POISSON", "CURRENT" };

	// -> 15 +/- Hz
	const float n_ma = 5.f; // MA(5), yes, float.
	const float eps_ma = 0.1f; // % stable, if < 0.1 Hz
	struct RESULT {
		RUN run;
		int mode; 
		float ma;  // Average Spiking Frequency
		std::deque<float> af; // samples
		int spikes;
		int samples; // Moving Average
	};

	const auto len = sizeof(RUN);
	RESULT results[TESTED_MODES][len];  // check if len or bytes

	int tested = 0;
	for (int mode = 0; mode < TESTED_MODES; mode++) {	 // start with CPU for dev/unit tests
	//for (int mode = TESTED_MODES - 1; mode >= 0;  mode--) {  // start with GPU for perf tests
	//{int mode = 1; // run single backend, 0 for CPU, 1 for GPU
		tested++;

		for (int run = RUN(POISSON); run <= RUN(CURRENT); run++) {
		//{int run = RUN(POISSON); 
		//{int run = RUN(CURRENT);

			// keep references local
			int neuronGroup[n_NT];  // todo init with -1  -> std:: array ... 
			int connGroup[n_NT][n_NT]; // pre, post

			CARLsim* sim = new CARLsim(mode ? "ca3_ntypes_GPU" : "ca3_ntypes_CPU", mode ? GPU_MODE : CPU_MODE, logger, numGPUs, randSeed);
			BACKEND_CORES = mode ? GPU_CORES : CPU_CORES;

			// ---------------- CONFIG STATE -------------------
			sim->setIntegrationMethod(RUNGE_KUTTA4, 5);

			// amount of neurons
			/*
			 *  0.025f * 74366^2 = 138,257,548 synapses => each neuron is connected to 1859 = 2.5%
       *  1000 -> 25 neuron
       *  100 -> 2.5 neurons 
       *  10 -> 0.25 -> n/a 
       *  125 -> 3 neurons
       *  400 -> 10 neurons
			*/
			int n = 125;   // unit test
			//int n = 400;	// functional tests
			//int n = 743;  // example
			//int n = 74366;	// CA3 mouse

			create_BC(sim, neuronGroup, n);
			EXPECT_GE(neuronGroup[BC], 0);
			
			// damps from 20 Hz to 2.3 Hz
			connect_BC_BC(sim, connGroup, neuronGroup); // use same weights and probability as the CA3 model
			EXPECT_GE(connGroup[BC][BC], 0);

			int g0; // input group
			float wt; // weight 
			PeriodicSpikeGenerator* spkGenG0 = nullptr;

			switch (run) {
			case RUN(POISSON): {
				// poisson source
				//float rate = 0.85f; // Hz
				//float rate = 10.0f;
				float rate = 12.75f;
				g0 = sim->createSpikeGeneratorGroup("input0", n, EXCITATORY_NEURON, -1, BACKEND_CORES);
				bool spikeAtZero = true;
				spkGenG0 = new PeriodicSpikeGenerator(rate, spikeAtZero); // periodic spiking @ <rate> Hz
				sim->setSpikeGenerator(g0, spkGenG0);

				//wt = 37.0f; // COBA
				wt = 40.0f; // COBA

				sim->connect(g0, neuronGroup[BC], "one-to-one", RangeWeight(0.0f, wt, 40.0f), 0.75f, // 0.0250664662231983f,   
					RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);

				sim->setSTP(g0, neuronGroup[BC], true,
					STPu(0.65f),
					STPtauU(75.0f),  // 750
					STPtauX(650.0f),   // 50
					//STPtdAMPA(15.0f, 0.0f),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
					STPtdAMPA(1.25 * 15.0f, 0.0f),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
					//STPtdAMPA(0.55, 0.0),   // 1.0 unchanged 14 Hz, 0.5 7 Hz, << 0.5 e.g. 0.1 dies, funny results for 0.49999f  353Hz 
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
				//float wt = 0.2f; // COBA  post syn group starts firing at 2000ms
				//wt = 1.2f; // COBA  post syn group starts firing at 1700ms
				//wt = 37.0f; // COBA
				wt = 40.0f; // COBA

				sim->connect(g0, neuronGroup[BC], "one-to-one", RangeWeight(0.0f, wt, 40.0f), 0.75, // 0.0250664662231983f,   
					RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);
				//RangeDelay(1), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);

				// Hint: STP false, respectively fixed synapses, are not supported by CS4_hc 
				// as the CA3 article finally focused on CSTP only.
				// STP = false behaves same way as if SYN_FIXED. 
				// CPU get activity from after a while (~2600ms), while GPU remain silent
				// if wt set to 0.0f then CPU also remains silent
				// 
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
				sim->setExternalCurrent(g0, 0.2f);  // 1500ms: 10,10,20 Hz
				//sim->setExternalCurrent(g0, 0.004f);  // 2000ms: 10,10,20 Hz
				//sim->setExternalCurrent(g0, 500.0f);  // 1100ms: 10,10,20 Hz
				break;
			default:
				;
			}

			SpikeMonitor* spkMon0 = sim->setSpikeMonitor(g0, "DEFAULT");
			SpikeMonitor* spkMon1 = sim->setSpikeMonitor(neuronGroup[BC], "DEFAULT");

			// ---------------- RUN STATE -------------------
			//nrnMon_Pyramidal1->startRecording();

			// run for a total of 10 seconds in 500ms bins
			// at the end of each runNetwork call, SpikeMonitor stats will be printed

			results[mode][run].mode = mode;
			results[mode][run].run = RUN(run);
			results[mode][run].spikes = -1;
			results[mode][run].ma = 0.0f;
			results[mode][run].samples = 0;

			try {
				for (int i = 0; i < 5 * 2; i++) {
					if (i == 0)
					{
						// run 20ms with stimulus 
						sim->runNetwork(0, 20, true);
           
						//silence stimulus
						switch (run) {
						case RUN(POISSON): {
							PoissonRate poissonRate(n, mode == GPU_MODE);
							poissonRate.setRates(0.0f);
							sim->setSpikeRate(g0, &poissonRate);
							break;
						}
						case RUN(CURRENT):
							sim->setExternalCurrent(g0, 0.0f);
							break;
						default:
							;
						}
            // complete first chunk 
						sim->runNetwork(0, 80, true);
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
								if (af > 0.1f) {  // Hz
									auto &afs = results[mode][run].af;
									afs.push_back(af);
									int n = afs.size(); 
									auto& ma = results[mode][run].ma;
									float ma_last = ma; 
									if (n == n_ma) {
										for (auto iter = afs.begin(); iter < afs.end(); iter++)
											ma += *iter / n_ma;
									}
									else if (n > n_ma) {
										ma += af / n_ma - afs.front() / n_ma;
										afs.pop_front();
									}
									//printf("%5d ms %s %s MeanFiringRate(BC) %.1f Hz MA(%d)=%.1f Hz\n", t, mode == GPU_MODE ? "GPU" : "CPU", run_names[run], af, n_ma, ma);
									if (ma_last > 0.0f && std::abs(ma - ma_last) < eps_ma)  // ma is stabelizing
										throw std::exception("done");
									results[mode][run].spikes = t;
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

		// both sensory input should stabilize the network
		EXPECT_NEAR(results[mode][POISSON].ma, results[mode][CURRENT].ma, 1.5f);  // MA(5) of AF in Hz
		EXPECT_NEAR(results[mode][POISSON].spikes, results[mode][CURRENT].spikes, 2000);  // 2.0s 
	}

	if(tested>1) {
		// compare GPU vs CPU by eps
		EXPECT_NEAR(results[GPU_MODE][POISSON].ma, results[CPU_MODE][POISSON].ma, 5.0f); // Hz
		EXPECT_NEAR(results[GPU_MODE][CURRENT].ma, results[CPU_MODE][CURRENT].ma, 5.0f); // Hz
		EXPECT_NEAR(results[GPU_MODE][POISSON].spikes, results[CPU_MODE][POISSON].spikes, 2000); // ms
		EXPECT_NEAR(results[GPU_MODE][CURRENT].spikes, results[CPU_MODE][CURRENT].spikes, 2000); // ms
	}


}
