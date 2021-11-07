
#ifdef LN_I_CALC_TYPES

#define SPK_MON

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>

#include <periodic_spikegen.h>
#include <spikegen_from_vector.h>

#include <vector>

/*!
 * \brief test config icalc on connections  
 * 
 * 2 pyramidical cells
 * 
 * at optimal levels, L3e keeps the state until the dicharge. 
 * set at 1000, typical dip after for 100ms, than consitent activity e.g. 100ms at 2.5s and 100ms 3.5 are equal
 * discharge at 4.0s reduce activity --> 0, after 200ms = 0
 * 
  NM = NetworkMonitor('./sim_adk13_a2d1.dat')
  NM.plot([1], 6000);
    
  % pc stimulus      
  NM.plot([11], 100);

  % crorolliary discharge
  NM.plot([9], 500);
  NM.plot([5], 1000);
 * 
 */
TEST(ADK13, wm) {

	const int n = 100;  // neurons in column
	const int columns = 2;  // columns

	//float baseNE = 0.0f;  // low level
	//float baseNE = 1.f/6.f;  // low level
	float baseNE = 0.5f;  // optimal level
	//float baseNE = 5.0f/6.0f;  // high level
	//float baseNE = 1.f;  // high level

	//float baseDA = 0.0f;  // low level
	//float baseDA = 1.0f/6.0f;  // low level
	float baseDA = 0.5f;  // optimal level
	//float baseDA = 5.0f/6.0f;  // high level

	//float tauNM = 50;
	//float releaseNM = 0.0005f;
	//int ne_drain = 2500;

	//float tauNM = 100;  // nice drain off
	//float releaseNM = 0.00026f;
	//int ne_drain = 2500;

	float tauNM = 200;  // nice drain off
	float releaseNM = 0.000125f;
	int ne_drain = 2500;

	//float tauNM = 250;  // no wm anymore
	//float releaseNM = 0.0001f;
	//int ne_drain = 2500;

	//float tauNM = 500;  // no wm anymore
	//float releaseNM = 0.00005f;
	//int ne_drain = 3500;

	//int ne_drain = 6000;


	std::vector<float> rates[TESTED_MODES];
	std::vector<float> expected[TESTED_MODES];
	int tested = 0;
	
	struct rs_s { float a = 0.01f; float b = 0.2f; float c = -65.0f; float d = 8.0f; } rs;  // regular spiking neuron 
	struct fs_s { float a = 0.1f; float b = 0.2f; float c = -65.0f; float d = 2.0f; } fs;  // fast spiking neuron 

	LoggerMode logger = SILENT; // SILENT USER DEVELOPER;
	const bool TRACE = false;

	for (int mode = 0; mode < TESTED_MODES; mode++) {
	//{int mode = 0;

		// first iteration, test CPU mode, second test GPU mode
		CARLsim* sim = new CARLsim("adk13_wm", mode ? GPU_MODE : CPU_MODE, logger, 1, 42);

		// COBA
		sim->setConductances(true);

		// PC/7a
		int g_PC7a[columns]; // 0, 90, // 180, 270

		// L2/3
		int g_L3e[columns]; // 0, 90, // 180, 270
		int g_L3i[columns]; // 0, 90, // 180, 270

		// L5/6
		int g_L5e[columns];
		int g_L5i[columns];

		// initialize columns at optimal level
		//for (int c = 0; c < columns; c++) {
		{int c = 0;

			g_PC7a[c] = sim->createGroup(std::string("PC7a_").append(std::to_string(c)), n, EXCITATORY_NEURON);
			sim->setNeuronParameters(g_PC7a[c], rs.a, rs.b, rs.c, rs.d);

			g_L3e[c] = sim->createGroup(std::string("L3e_").append(std::to_string(c)), n, EXCITATORY_NEURON); // 2585
			sim->setNeuronParameters(g_L3e[c], rs.a, rs.b, rs.c, rs.d); 

			g_L3i[c] = sim->createGroup(std::string("L3i_").append(std::to_string(c)), n, INHIBITORY_NEURON);  // 729
			sim->setNeuronParameters(g_L3i[c], fs.a, fs.b, fs.c, fs.d);

			g_L5e[c] = sim->createGroup(std::string("L5e_").append(std::to_string(c)), n, EXCITATORY_NEURON); // 606
			sim->setNeuronParameters(g_L5e[c], rs.a, rs.b, rs.c, rs.d);

			g_L5i[c] = sim->createGroup(std::string("L5i_").append(std::to_string(c)), n, INHIBITORY_NEURON); // 133
			sim->setNeuronParameters(g_L5i[c], fs.a, fs.b, fs.c, fs.d);

			sim->setNeuromodulator(g_L3e[c],
				baseDA, tauNM, releaseNM, true, // da
				0.001f, 1.f, 0.f, false,
				0.001f, 1.f, 0.f, false,
				baseNE, tauNM, releaseNM, true); // ne

			// connect column
			int c_id = sim->connect(g_L3e[c], g_L3e[c], "random", RangeWeight(0.04725f), 0.025f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // 0.3584f
			//int c_id = sim->connect(g_L3e[c], g_L3e[c], "random", RangeWeight(0.04725f), 0.025f, RangeDelay(1), RadiusRF(-1), SYN_FIXED, 0.1f, 15.0f);  // 0.3584f
			//int c_id = sim->connect(g_L3e[c], g_L3e[c], "random", RangeWeight(0.04725f), 0.025f, RangeDelay(1), RadiusRF(-1), SYN_FIXED, 0.1f, 10.65f);  // 0.3584f
			//printf("connection-id( L3e[%d] --> L3e[%d] ) = %d\n", c, c, c_id);
			sim->connect(g_L3e[c], g_L3i[c], "random", RangeWeight(0.04f), 0.01f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // 0.1008f
			//sim->connect(g_L5e[c], g_L5e[c], "full", RangeWeight(0.4f), 0.0758f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			//sim->connect(g_L5e[c], g_L5i[c], "full", RangeWeight(0.4f), 0.0566f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			sim->connect(g_L3i[c], g_L3e[c], "random", RangeWeight(0.4f), 0.01f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // 0.1552f,
			sim->connect(g_L3i[c], g_L3i[c], "random", RangeWeight(0.2f), 0.01f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);	// 0.1371f
			//sim->connect(g_L5i[c], g_L5e[c], "full", RangeWeight(0.4f), 0.3765f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			//sim->connect(g_L5i[c], g_L5i[c], "full", RangeWeight(0.4f), 0.3158f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

			// connect other 1:1
			sim->connect(g_PC7a[c], g_L3e[c], "one-to-one", RangeWeight(0.1f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			sim->connect(g_PC7a[c], g_L3i[c], "one-to-one", RangeWeight(0.1f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

			sim->setConnectionModulation(g_L3e[c], g_L3e[c], alpha2A_ADK13);
		}

		// connect groups 

		// Simulus
		// create spike gen from vector
		std::vector<int> spikeTimes;
		for (int t = 1000; t < 1500; t += 25) spikeTimes.push_back(t); // 40Hz at 1s for 500ms
		//int g_ctx = sim->createSpikeGeneratorGroup("ctx", n, EXCITATORY_NEURON);
		int g_ctx = sim->createSpikeGeneratorGroup("ctx", 1, EXCITATORY_NEURON);
		SpikeGeneratorFromVector spkGen(spikeTimes);
		sim->setSpikeGenerator(g_ctx, &spkGen);
		//sim->connect(g_ctx, g_PC7a[1], "one-to-one", RangeWeight(4.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  
		//sim->connect(g_ctx, g_PC7a[1], "full", RangeWeight(0.4f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(g_ctx, g_PC7a[0], "full", RangeWeight(0.4f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

		// BG - WM reset 
		std::vector<int> clearWM;
		for (int t = 4000; t < 4500; t += 25) clearWM.push_back(t); // 40Hz at 1s for 500ms
		int g_bg_gen = sim->createSpikeGeneratorGroup("bg", 1, EXCITATORY_NEURON);
		SpikeGeneratorFromVector spkGen2(clearWM);
		sim->setSpikeGenerator(g_bg_gen, &spkGen2);
		//int g_bg = sim->createGroup(std::string("bg"), n, INHIBITORY_NEURON);  // 729
		int g_bg = sim->createGroup(std::string("bg"), n, EXCITATORY_NEURON);  // 729
		sim->setNeuronParameters(g_bg, fs.a, fs.b, fs.c, fs.d);
		sim->connect(g_bg_gen, g_bg, "full", RangeWeight(0.4f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		//sim->connect(g_bg, g_L3i[1], "one-to-one", RangeWeight(0.4f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED); // >= 0.045 clear cut-off
		sim->connect(g_bg, g_L3i[0], "one-to-one", RangeWeight(0.4f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED); // >= 0.045 clear cut-off

		//// L3_npref  - CAUTION only for testing config, requrires other columns
		//std::vector<int> nprefL3;
		//for (int t = 1000; t < 4000; t += 50) nprefL3.push_back(t); // 40Hz at 1s for 500ms
		//int g_vta = sim->createSpikeGeneratorGroup("vta", 1, EXCITATORY_NEURON);  
		//SpikeGeneratorFromVector spkGen3(nprefL3);
		//sim->setSpikeGenerator(g_vta, &spkGen3);
		//int g_L3e_npref = sim->createGroup(std::string(""), n, EXCITATORY_NEURON);  // 729
		//sim->setNeuronParameters(g_L3e_npref, rs.a, rs.b, rs.c, rs.d);
		//sim->connect(g_vta, g_L3e_npref, "full", RangeWeight(0.4f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		//sim->connect(g_L3e_npref, g_L3e[0], "one-to-one", RangeWeight(0.1f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		//sim->setConnectionModulation(g_L3e_npref, g_L3e[0], D1_ADK13);


		// locus coeruleus
		// raise ne for 0.5s and hold decay at 2500 
		std::vector<int> lc;
		for (int t = 1; t < ne_drain; t += 5) lc.push_back(t); // 40Hz at 1s for 500ms
		int g_lc = sim->createSpikeGeneratorGroup("g_lc", 1, NORADRENERGIC_NEURON);  
		SpikeGeneratorFromVector spkGen4(lc);
		sim->setSpikeGenerator(g_lc, &spkGen4);
		sim->connect(g_lc, g_L3e[0], "full", RangeWeight(.0f), 0.4f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

		// setup monitoring
		std::vector<GroupMonitor*> gm;
		std::vector<SpikeMonitor*> sm;

		gm.push_back(sim->setGroupMonitor(g_L3e[0], "DEFAULT", 1));

		//for (int c = 0; c < columns; c++) {
		{int c = 0; 
			sm.push_back(sim->setSpikeMonitor(g_PC7a[c], "DEFAULT"));
			sm.push_back(sim->setSpikeMonitor(g_L3e[c], "DEFAULT"));
			sm.push_back(sim->setSpikeMonitor(g_L3i[c], "DEFAULT"));
			//sm.push_back(sim->setSpikeMonitor(g_L3e_npref, "DEFAULT"));			
		}
		//sm.push_back(sim->setSpikeMonitor(g_ctx, "DEFAULT"));
		sm.push_back(sim->setSpikeMonitor(g_bg, "DEFAULT"));

		// SETUP STATE 
		sim->setupNetwork();

		// RUN STATE 
		for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->startRecording(); });

		// LN2021 \todo refact lambda 
		if (TRACE) printf("%s:\n", mode ? "GPU" : "CPU");

		// raise ne level to opt (0.5) in 0.5s
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });
		sim->runNetwork(1, 0, true);
		// effectively the same as printRunSummary = true, however, for unit testing the values are required for comparison
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
		rates[mode].push_back(sim->getSpikeMonitor(g_L3e[0])->getPopMeanFiringRate());
		if (TRACE) printf("%4d L3e_%d mean firing rate: %.2f Hz\n", 1000, 0, rates[mode].back());
		//sim->getSpikeMonitor(g_L3e[0])->print(false);
		if (logger < SILENT) printf("\n");

		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });
		sim->runNetwork(0, 500, true);
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
		rates[mode].push_back(sim->getSpikeMonitor(g_L3e[0])->getPopMeanFiringRate());
		if (TRACE) printf("%4d L3e_%d mean firing rate: %.2f Hz\n", 500, 0, rates[mode].back());
		//sim->getSpikeMonitor(g_L3e[0])->print(false);
		if (logger < SILENT) printf("\n");

		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });
		sim->runNetwork(0, 500, true);
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
		rates[mode].push_back(sim->getSpikeMonitor(g_L3e[0])->getPopMeanFiringRate());
		if(TRACE) printf("%4d L3e_%d mean firing rate: %.2f Hz\n", 500, 0, rates[mode].back());
		if (logger < SILENT) printf("\n");

		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });
		sim->runNetwork(2, 0, true);
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
		rates[mode].push_back(sim->getSpikeMonitor(g_L3e[0])->getPopMeanFiringRate());
		if (TRACE) printf("%4d L3e_%d mean firing rate: %.2f Hz\n", 2000, 0, rates[mode].back());
		if (logger < SILENT) printf("\n");

		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });
		sim->runNetwork(0, 500, true);
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
		rates[mode].push_back(sim->getSpikeMonitor(g_L3e[0])->getPopMeanFiringRate());
		if (TRACE) printf("%4d L3e_%d mean firing rate: %.2f Hz\n", 500, 0, rates[mode].back());
		if (logger < SILENT) printf("\n");

		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });
		sim->runNetwork(0, 500, true);
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
		rates[mode].push_back(sim->getSpikeMonitor(g_L3e[0])->getPopMeanFiringRate());
		if (TRACE) printf("%4d L3e_%d mean firing rate: %.2f Hz\n", 500, 0, rates[mode].back());
		if (logger < SILENT) printf("\n");

		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });
		sim->runNetwork(2, 0, true);
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
		rates[mode].push_back(sim->getSpikeMonitor(g_L3e[0])->getPopMeanFiringRate());
		if (TRACE) printf("%4d L3e_%d mean firing rate: %.2f Hz\n", 2000, 0, rates[mode].back());

		for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->stopRecording(); });

		delete sim;
		
		/* sample result at optimal levels
		1000 L3e_0 mean firing rate: 0.00 Hz
		 500 L3e_0 mean firing rate: 9.04 Hz
		 500 L3e_0 mean firing rate: 2.90 Hz
		2000 L3e_0 mean firing rate: 8.65 Hz
		 500 L3e_0 mean firing rate: 0.82 Hz
		 500 L3e_0 mean firing rate: 0.00 Hz
		2000 L3e_0 mean firing rate: 0.00 Hz
		*/
		expected[mode] = { 0.0, 9.0, 2.9, 8.6, 0.8, 0.0, 0.0 };
		for (int i = 0; i < expected[mode].size(); i++)
			EXPECT_NEAR(rates[mode][i], expected[mode][i], 1.5f);  // abs_error can be increased

		tested++;
	}

	if (tested > 0) {

	}
}


#endif
