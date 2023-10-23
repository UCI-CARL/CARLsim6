
#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>

#include <stopwatch.h>

#include <periodic_spikegen.h>
#include <spikegen_from_vector.h>
#include <interactive_spikegen.h>

#include <vector>

#include <algorithm>
#include <random>
#include <cmath>



// ---------------- CONFIG STATE -------------------

TEST(neurongroups, limit) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	for (int mode = GPU_MODE; mode >= CPU_MODE; mode--) {

		LoggerMode logger = SILENT;
		//LoggerMode logger = USER;

		bool bSpikeMon = true;

		CARLsim sim("nrngrps_limit", mode ? GPU_MODE : CPU_MODE, logger);

		// Neurons
		int NRNS = 7000; // neurons    GB device memory, 210 * 7000 neurons
		//int NRNS = 100; // neurons

		int g_in = sim.createSpikeGeneratorGroup("in", 1, EXCITATORY_NEURON);

		const int GRPS = 210;
		//const int GRPS = 10;
		//const int GRPS = 310; // expected death 

		const int CONNS = 32000;   // int16 adressing

		int g_exc[GRPS];
		for (int i = 0; i < GRPS; i++) {
			std::string name = "exc" + std::to_string(i);
			g_exc[i] = sim.createGroup(name, NRNS, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
			sim.setNeuronParameters(g_exc[i], 0.02f, 0.2f, -65.0f, 8.0f); // RS
		}

		// Connections
		sim.connect(g_in, g_exc[0], "full", RangeWeight(40), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

		// Forward
		for (int i = 1; i < GRPS; i++) {
			sim.connect(g_exc[i-1], g_exc[i], "one-to-one", RangeWeight(40), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		}

		// Full 
		int g_conn = 0; 
		for (int i = 0; i < GRPS; i++) {
			for (int j = 0; j < GRPS; j++) {
				if (j != i - 1 && j != i   //ignore recurrent
					&& g_conn < CONNS 
					) 
				{
					auto w = 20.f + 5.0f/GRPS;  // normalize avg firing rate
					g_conn = sim.connect(g_exc[i], g_exc[j], "one-to-one", RangeWeight(w), 1.0f, RangeDelay(1, 20), RadiusRF(-1), SYN_FIXED);
				}
			}
		}

		// Sensory input
		std::vector<int> spikeTimes_exc;
		int interval = 100; // ms
		int iterations = 5; 
		for (int i = 0; i < iterations; i++) {
			spikeTimes_exc.push_back(1 + i*interval);
		}
	
		SpikeGeneratorFromVector spkGen_exc(spikeTimes_exc);
		sim.setSpikeGenerator(g_in, &spkGen_exc);

		// avoid warning  setConductances has not been called. Setting simulation mode to CUBA.
		sim.setConductances(false);

		// ---------------- SETUP STATE -------------------
#ifdef VLS_SIZING
		if (GRPS <= 210)
			sim.setupNetwork();
		else {
			EXPECT_DEATH(sim.setupNetwork(), ""); // CUDA error code=1(cudaErrorInvalidValue) "cudaMemcpyToSymbol(groupConfigsGPU, 
			return;
		}
#else		
		if (GRPS <= 128)
			sim.setupNetwork();
		else {
			EXPECT_DEATH(sim.setupNetwork(), "");  // SEH exception with code 0xc0000005 
			return;
		}
#endif

		// generateSETUPStateSTP.h
		for (int i = 0; i < GRPS; i++) {
			if (bSpikeMon) {
				auto sm = sim.setSpikeMonitor(g_exc[i], "DEFAULT");
				sm->startRecording();
			}
		}

		// ---------------- RUN STATE -------------------
		sim.runNetwork(0, 10, true);

		if (bSpikeMon) {
			float totalRate = .0f;
			int n = 0;
			for (int i = 4; i < GRPS; i++) {  // forward only
				auto sm = sim.getSpikeMonitor(g_exc[i]);
				sm->stopRecording(); 
				//auto rate = sm->getMaxFiringRate();  // over all monitors ?
				auto rate = sm->getPopMeanFiringRate();
				//printf("firing rate exc%02d:  %.2f Hz\n", g_exc[i], rate); 
				auto spikes = sm->getPopNumSpikes();
				totalRate += rate;
				n++;
			}
			float avgRate = totalRate / n;
			printf("avg firing rate (%s):  %.2f Hz\n", mode == GPU_MODE ? "GPU" : "CPU", avgRate);
			EXPECT_NEAR(avgRate, 9.0f, 5.0f);
		}

	}
	
}


