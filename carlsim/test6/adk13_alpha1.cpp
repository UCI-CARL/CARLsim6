
#ifdef LN_I_CALC_TYPES

#define SPK_MON

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>

#include <periodic_spikegen.h>
#include <spikegen_from_vector.h>

#include <vector>

#include <cmath>


/*!
 * \brief test config icalc on connections  
 * 
 * 2 pyramidical cells
 * 
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
TEST(ADK13, alpha1) {

	const int n = 100;  // neurons in column
	const int columns = 2;  // columns

	//// cases: (low,opt,high)^#(DA,NE) = 9
	//// matrix DA-levels -> row, NE-levels -> columns ADK13 Table 4, 5
	//// e.g. (lowDA,highNE) -> (1,3) = [0][2]
	//struct szenario {
	//	//std::string desc;
	//	//Level levelNE, levelDA;  // symbolic level -> matrix index
	//	float baseNE, baseDA; // level
	//	float tauNM;  // no wm anymore
	//	float releaseNM;
	//	int ne_drain;
	//};


	//float baseNE = 0.0f;  // low level
	//float baseNE = 1.f/6.f;  // low level
	//float baseNE = 0.5f;  // optimal level
	float baseNE = 5.0f/6.0f;  // high level
	//float baseNE = 1.f;  // high level

	//float baseDA = 0.0f;  // low level
	//float baseDA = 1.0f/6.0f;  // low level
	//float baseDA = 0.5f;  // optimal level
	float baseDA = 5.0f/6.0f;  // high level

	//float tauNM = 50;
	//float releaseNM = 0.0005f;
	//int ne_drain = 2500;

	//float tauNM = 100;  // nice drain off
	//float releaseNM = 0.00026f;
	//int ne_drain = 2500;

	//float tauNM = 200;  // nice drain off
	//float releaseNM = 0.000125f;
	//int ne_drain = 2500;

	//float tauNM = 250;  // no wm anymore
	//float releaseNM = 0.0001f;
	//int ne_drain = 2500;

	float tauNM = 500;  // no wm anymore
	float releaseNM = 0.00005f;
	int ne_drain = 3500;

	//int ne_drain = 6000;
	

	struct rs_s { float a = 0.01f; float b = 0.2f; float c = -65.0f; float d = 8.0f; } rs;  // regular spiking neuron 
	struct fs_s { float a = 0.1f; float b = 0.2f; float c = -65.0f; float d = 2.0f; } fs;  // fast spiking neuron 


	LoggerMode logger = SILENT; // SILENT USER DEVELOPER;
	{int mode = 0;

		// first iteration, test CPU mode, second test GPU mode
		CARLsim* sim = new CARLsim("adk13_alpha1", mode ? GPU_MODE : CPU_MODE, logger, 1, 42);

		// COBA
		//sim->setDefaultConductanceTimeConstants(5, 0, 100, 6, 0, 150);	// p4 ADK13, tau_NMDA = 100 (default = 150) //LN no principal change, but requrires new param fitting
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
			// _mu_DA,NE,grp
			sim->setNM4weighted(g_L3e[c], alpha1_ADK13, 1.f, 0.f, 0.f, 1.0f, 1.f, -1.0f / 6.0f / std::log(1.0f / 3.0f));

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
			int c_id = sim->connect(g_L3e[c], g_L3e[c], "random", RangeWeight(0.05f), 0.025f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // 0.3584f
			//fix alpha2A at low level for fitting of alpha1
			//int c_id = sim->connect(g_L3e[c], g_L3e[c], "random", RangeWeight(0.04725f), 0.025f, RangeDelay(1), RadiusRF(-1), SYN_FIXED, 0.1f, 10.65f);  // 0.3584f
			//fix alpha2A at optimal level for fitting of alpha1
			//int c_id = sim->connect(g_L3e[c], g_L3e[c], "random", RangeWeight(0.04725f), 0.025f, RangeDelay(1), RadiusRF(-1), SYN_FIXED, 0.1f, 15.0f);  // 0.3584f
			//fix alpha2A at optimal level for fitting of alpha1
			//int c_id = sim->connect(g_L3e[c], g_L3e[c], "random", RangeWeight(0.05), 0.025f, RangeDelay(1), RadiusRF(-1), SYN_FIXED, 0.1f, 14.0f);  // 0.3584f
			//printf("connection-id( L3e[%d] --> L3e[%d] ) = %d\n", c, c, c_id);
			sim->connect(g_L3e[c], g_L3i[c], "random", RangeWeight(0.04f), 0.01f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // 0.1008f
			//sim->connect(g_L5e[c], g_L5e[c], "full", RangeWeight(0.4f), 0.0758f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			//sim->connect(g_L5e[c], g_L5i[c], "full", RangeWeight(0.4f), 0.0566f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			sim->connect(g_L3i[c], g_L3e[c], "random", RangeWeight(0.4f), 0.01f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // 0.1552f,
			sim->connect(g_L3i[c], g_L3i[c], "random", RangeWeight(0.2f), 0.01f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);   // 0.1371f
			//sim->connect(g_L5i[c], g_L5e[c], "full", RangeWeight(0.4f), 0.3765f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			//sim->connect(g_L5i[c], g_L5i[c], "full", RangeWeight(0.4f), 0.3158f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

			// connect other 1:1
			sim->connect(g_PC7a[c], g_L3e[c], "one-to-one", RangeWeight(0.1f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
			sim->connect(g_PC7a[c], g_L3i[c], "one-to-one", RangeWeight(0.1f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

			// fix for alpha1
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
		//int g_L3e_npref_stim = sim->createSpikeGeneratorGroup("L3e_npref_stim", 1, EXCITATORY_NEURON);
		//SpikeGeneratorFromVector spkGen3(nprefL3);
		//sim->setSpikeGenerator(g_L3e_npref_stim, &spkGen3);
		//int g_L3e_npref = sim->createGroup(std::string(""), n, EXCITATORY_NEURON);  // 729
		//sim->setNeuronParameters(g_L3e_npref, rs.a, rs.b, rs.c, rs.d);
		//sim->connect(g_L3e_npref_stim, g_L3e_npref, "full", RangeWeight(0.4f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
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
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });

		// raise ne level to opt (0.5) in 0.5s
		sim->runNetwork(1, 0, true);
		if (logger < SILENT) printf("\n");
		sim->runNetwork(0, 500, true);
		if (logger < SILENT) printf("\n");
		sim->runNetwork(0, 500, true);
		if (logger < SILENT) printf("\n");
		sim->runNetwork(2, 0, true);
		if (logger < SILENT) printf("\n");
		sim->runNetwork(0, 500, true);
		if (logger < SILENT) printf("\n");
		sim->runNetwork(0, 500, true);
		if (logger < SILENT) printf("\n");
		sim->runNetwork(2, 0, true);

		for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->stopRecording(); });
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });

		delete sim;
	}

}




#endif
