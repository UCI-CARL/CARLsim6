
#ifdef LN_I_CALC_TYPES

#define SPK_MON

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>

#include <periodic_spikegen.h>
#include <spikegen_from_vector.h>

#include <vector>

/*!
 * \brief testing CARLsim6 NM4 receptors yielding convergent and divergent actions, 
 *
 * [Nadim 2014] Neuromodulation of Neurons and Synapses, Farzan Nadim and Dirk Bucher 2014   
 * 
 * test tonic to phasic, bursting/chattering
 * with monitoring
 *   
 * test w0 -> no activity, nm-ergic, target
 * test monitoring
 *  
 * connect all together build network      
 * 
 */

TEST(Receptors, fig2b) {

	int n = 12;

	float baseDA = 0.001f;
	float baseNE = 0.001f;
	float tauDA = 50;
	//float tauNE = 100;
	float tauNE = 50;
	float releaseDA = 0.02f;
	float releaseNE = 0.03f;
	//float releaseNE = 0.0001f;
	float f = 100.f; // release frequency in Hz 

	float a = 0.02f, b = 0.2f, c = -55.0f, d = 4.0f; // IB
	//float a = 0.02f, b = 0.2f, c = -52.5f, d = 3.0f; // LN

	const int n_id = 10;
	const int grps = 4;
	const int phases = 3;
	int spikes[TESTED_MODES][phases][grps];

	LoggerMode logger = SILENT; // USER DEVELOPER;

	int tested = 0;
	for (int mode = 0; mode < TESTED_MODES; mode++) {
	//{int mode = 0;

		// first iteration, test CPU mode, second test GPU mode
		CARLsim* sim = new CARLsim("receptors_fig2b", mode ? GPU_MODE : CPU_MODE, logger, 1, 42);


		// CUBA
		sim->setConductances(false);

		//nm-targets

		// control without nm projections
		int g_control = sim->createGroup("control", n, EXCITATORY_NEURON);

		// nm1 DA
		int g_nm1 = sim->createGroup("nm1", n, EXCITATORY_NEURON);

		// nm2 NE
		int g_nm2 = sim->createGroup("nm2", n, EXCITATORY_NEURON);

		// nm12 DA,NE
		int g_nm12 = sim->createGroup("nm12", n, EXCITATORY_NEURON);

		std::vector<int> groups = { g_control, g_nm1, g_nm2, g_nm12 };

	// TODO DA enhances, SE damps, DA + SE even, BUT goal is fig2b !!!

		// LN2021 6th param! -> base default 1.0f, can be weighted, normalized, and boosted -> then its complete!!! -> long live linear combinations

		// synergistic Farzan Nadim, Dirk Bucher 2014
		float w[] = { 
				1.0f/1.5f,	// DA  normalize level and then weight
				0.0f, 
				0.0f, 
				2.0f/1.5f,	// NE  normalize level and then weight
				4.0f / (1.0f + 2.0f),  // normalize incl. base and boost
				1.0f		// unmodulated baseline 
		};

		//// LN initial learning, 4. damps, 1. enhances, both compensate each other (antagoistic Nadin/Bucher)
		//float w[] = {
		//		1.0f/1.5f,	// DA  normalize level and then weight
		//		0.0f,
		//		0.0f,
		//		-1.0f/1.5f,	// NE  normalize level and then weight
		//		4.0f/(2.0f),  // normalize incl. base and boost
		//		2.0f		// unmodulated baseline 
		//};

		// Sigmoid


		sim->setNM4weighted(g_nm1,  NM4W_LN21, w[0], w[1], w[2], w[3], w[4]);
		sim->setNM4weighted(g_nm2,  NM4W_LN21, w[0], w[1], w[2], w[3], w[4]);
		sim->setNM4weighted(g_nm12, NM4W_LN21, w[0], w[1], w[2], w[3], w[4]);


		for_each(groups.begin(), groups.end(), [&](int g) {
			sim->setNeuronParameters(g, a, b, c, d); // IB
			sim->setNeuromodulator(g,
				baseDA, tauDA, releaseDA, true,
				0.001f, 1.f, 0.f, false,
				0.001f, 1.f, 0.f, false,
				baseNE, tauNE, releaseNE, true);
			});


		//nm-ergic

		// nm1 DA
		int g_nm1ergic = sim->createGroup("nm1ergic", 1, DOPAMINERGIC_NEURON);
		sim->setNeuronParameters(g_nm1ergic, 0.02f, 0.2f, -65.0f, 8.0f); // RS

		// nm2 NE
		int g_nm2ergic = sim->createGroup("nm2ergic", 1, NORADRENERGIC_NEURON);
		sim->setNeuronParameters(g_nm2ergic, 0.02f, 0.2f, -65.0f, 8.0f); // RS


		// w = 0 => do not trigger neural activity directly
		sim->connect(g_nm1ergic, g_nm1, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(g_nm2ergic, g_nm2, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(g_nm1ergic, g_nm12, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(g_nm2ergic, g_nm12, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);


		std::vector<float> current;
		for (int i = 0; i < n; i++)
			current.push_back(i * 1.f);  // \todo test that without no activity due weight 0 


		// use periodic spike generator to know the exact dopamine delivery
		// cortex downstream activation
		int g_ctx = sim->createSpikeGeneratorGroup("ctx", 1, EXCITATORY_NEURON);  

		// \todo CARLsim5 bug: Non-zero minimum weights are not yet supported.
		// \todo CARLsim6 feature: derive sublacces like SpikeGen to set the weights from file, vector, external source / yarp
		// RangeWeight(150.0f, 150.0f)
		//connect("ctx","nm1ergic"): wt=[150,150,150]. Non-zero minimum weights are not yet supported.
		//Assertion failed : false, file C : \cockroach - ut3\src\CARLsim6\carlsim\interface\src\carlsim.cpp, line 188
		// Workaound: fry it by 1000.0f 

		sim->connect(g_ctx, g_nm1ergic, "one-to-one", RangeWeight(60.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // shall trigger exactly on spike and therefore NM release
		sim->connect(g_ctx, g_nm2ergic, "one-to-one", RangeWeight(60.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // shall trigger exactly on spike and therefore NM release

		std::vector<int> groups2 = { g_ctx, g_nm1ergic, g_nm2ergic};

		// ctx activity
		// downstream activation

		//PoissonRate poissRate(1, mode);         // allocate on GPU for minimal memory copies

		//PeriodicSpikeGenerator* spkGen(f, false);

		// create spike gen from vector
		std::vector<int> spikeTimes; 
		for (int t = 100; t < 300; t++) spikeTimes.push_back(t);
		for (int t = 600; t < 800; t++) spikeTimes.push_back(t);

		SpikeGeneratorFromVector spkGen(spikeTimes);

		sim->setSpikeGenerator(g_ctx, &spkGen);


		// SETUP STATE 
		sim->setupNetwork();

		// setup monitoring
		std::vector<GroupMonitor*> gm;
		for_each(groups.begin(), groups.end(), [&](int g) {
			gm.push_back(sim->setGroupMonitor(g, "DEFAULT", 1));
			});

		std::vector<SpikeMonitor*> sm;
		for_each(groups.begin(), groups.end(), [&](int g) {
			sm.push_back(sim->setSpikeMonitor(g, "DEFAULT"));
			});
		for_each(groups2.begin(), groups2.end(), [&](int g) {
			sm.push_back(sim->setSpikeMonitor(g, "DEFAULT"));
		});

		// excite 
		for_each(groups.begin(), groups.end(), [&](int g) {
			sim->setExternalCurrent(g, current);
			});

		// RUN STATE 
		for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->startRecording(); });
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });

		//poissRate.setRates(0);					// set all rates to 0 Hz
		//sim->setSpikeRate(g_ctx, &poissRate);
		sim->runNetwork(0, 100, false);

		//poissRate.setRates(f);					// set all rates to f Hz
		//sim->setSpikeRate(g_ctx, &poissRate);
		sim->runNetwork(0, 800, false);

		//poissRate.setRates(0);					// set all rates to 0 Hz
		//sim->setSpikeRate(g_ctx, &poissRate);
		sim->runNetwork(0, 100, false);

		for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->stopRecording(); });
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });




		int phase = 0;  // \todo struct
		for (int g = 0; g < grps; g++) {
			spikes[mode][phase][g] = 0;
			auto sm_g = sm[g]->getSpikeVector2D()[n_id];
			int phase_start = 0;
			int phase_end = 100;
			for_each(sm_g.begin(), sm_g.end(), [&](int spikeTime) {
				if (spikeTime >= phase_start && spikeTime < phase_end) {
					//printf("phase[%d] g[%d] has spike at t=%d\n", phase, g, spikeTime);
					spikes[mode][phase][g]++;
				}
			});
		}

		phase++;  // \todo struct
		for (int g = 0; g < grps; g++) {
			spikes[mode][phase][g] = 0;
			auto sm_g = sm[g]->getSpikeVector2D()[n_id];
			int phase_start = 100;
			int phase_end = 300;
			for_each(sm_g.begin(), sm_g.end(), [&](int spikeTime) {
				if (spikeTime >= phase_start && spikeTime < phase_end) {
					//printf("phase[%d] g[%d] has spike at t=%d\n", phase, g, spikeTime);
					spikes[mode][phase][g]++;
				}
				});
		}

		phase++;  // \todo struct
		for (int g = 0; g < grps; g++) {
			spikes[mode][phase][g] = 0;
			auto sm_g = sm[g]->getSpikeVector2D()[n_id];
			int phase_start = 400;
			int phase_end = 500;
			for_each(sm_g.begin(), sm_g.end(), [&](int spikeTime) {
				if (spikeTime >= phase_start && spikeTime < phase_end) {
					//printf("phase[%d] g[%d] has spike at t=%d\n", phase, g, spikeTime);
					spikes[mode][phase][g]++;
				}
				});
		}

		/*
		* phase 0 (spikes/group):  4   4   4   4
		* phase 1 (spikes/group):  6  21  36  50
		* phase 2 (spikes/group):  3   3   3   3
		*/
		//for (int phase = 0; phase < 3; phase++) {
		//	printf("phase %d on %s (spikes/group): ", phase, mode?"GPU":"CPU");
		//	for (int g = 0; g < grps; g++) {
		//		printf(" %d ", spikes[mode][phase][g]);
		//	}
		//	printf("\n");
		//}

		// in phase 0 activity must be equal to the control group, as there is no NM yet
		phase = 0; 
		for (int g = 1; g < grps; g++) {
			EXPECT_EQ(spikes[mode][phase][0], spikes[mode][phase][g]);
		}
		// in phase 1 the activity must be significantly higher compared relatively as the nm is synergistic (aka convergent actions)
		phase = 1;
		for (int g = 1; g < grps; g++) {
			EXPECT_GT(spikes[mode][phase][g], spikes[mode][phase][g-1]+10);			// \todo fails with recent changes
		}
		// and phase 2 the activity must be equal to the control group again, as there is no NM anymore 
		phase = 2;
		for (int g = 1; g < grps; g++) {
			EXPECT_EQ(spikes[mode][phase][0], spikes[mode][phase][g]); // in phase 0 and 2 the activity must be equal to the control group, as no NM is there anymore 
		}

		delete sim;

		tested++;
	}

	// CPU vs GPU
	if (tested == 2) {  // first (CPU) is reference
		for (int phase = 0; phase < 3; phase++)
			for (int g = 0; g < grps; g++)
				EXPECT_NEAR(spikes[0][phase][g], spikes[1][phase][g], 0.5f); ;
	}

}

/*!
 * \brief test new icalc 
 * 
 * test icalc gpcrs1 Nadim	2014 gpcrs1   G protein-coupled receptors (GPCRs) 
 * https://www.promega.com/applications/small-molecule-drug-discovery/gpcr-research-drug-discovery
 * https://en.wikipedia.org/wiki/G_protein-coupled_receptor
 * [Breedlove 9th], p 99 Fig 4.2, right, \sa metabotropic receptors dar1,.. 
 * 
 * test without the dopminergic path by setting specific base line
 * test config of IcalcType to NM4W_LN21
 * test with I_ext 
 * test with g_sink
 */
TEST(Receptors, nm4w) {

	int n = 20;

	float baseDA = 3.f;	
	float baseNE = 2.f;
	float tauNM = 50;
	float releaseNM = 0.f;
	int f = 100; // release frequency in Hz 

	float a = 0.02f, b = 0.2f, c = -55.0f, d = 4.0f; // IB
	//float a = 0.02f, b = 0.2f, c = -52.5f, d = 3.0f; // LN

	int tested = 0;
	float rates[TESTED_MODES][5];

	LoggerMode logger = SILENT; // USER DEVELOPER;

	for (int mode = 0; mode < TESTED_MODES; mode++) {
	//{int mode = 0;

		// first iteration, test CPU mode, second test GPU mode
		CARLsim* sim = new CARLsim("receptors_nm4w", mode ? GPU_MODE : CPU_MODE, logger, 1, 42);

		//nm-targets

		// control without nm projections
		int g_control = sim->createGroup("control", n, EXCITATORY_NEURON);

		// nm1 DA
		int g_nm1 = sim->createGroup("nm1", n, EXCITATORY_NEURON);

		// nm2 NE
		int g_nm2 = sim->createGroup("nm2", n, EXCITATORY_NEURON);

		// nm12 DA,NE
		int g_nm12 = sim->createGroup("nm12", n, EXCITATORY_NEURON);

		std::vector<int> groups = { g_control, g_nm1, g_nm2, g_nm12 };

		sim->setNM4weighted(g_nm1, NM4W_LN21, 1.f, 0.f, 0.f, 0.f, 1.f);   
		sim->setNM4weighted(g_nm2, NM4W_LN21, 0.f, 0.f, 0.f, 2.f, 1.f);
		sim->setNM4weighted(g_nm12, NM4W_LN21, 1.f, 0.f, 0.f, -1.f, 1.f);


		for_each(groups.begin(), groups.end(), [&](int g) {
			sim->setNeuronParameters(g, a, b, c, d); // IB
			sim->setNeuromodulator(g,
				baseDA, tauNM, releaseNM, true,
				0.001f, 1.f, 0.f, false,
				0.001f, 1.f, 0.f, false,
				baseNE, tauNM, releaseNM, true);
			});

		// sink
		int g_sink = sim->createGroup("sink", 1, EXCITATORY_NEURON);
		sim->setNeuronParameters(g_sink, 0.02f, 0.2f, -65.0f, 8.0f); // RS

		// dummy connection
		sim->connect(g_control,	g_sink, "full", RangeWeight(1.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(g_nm1,		g_sink, "full", RangeWeight(1.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(g_nm2,		g_sink, "full", RangeWeight(1.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(g_nm12,	g_sink, "full", RangeWeight(1.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);


		std::vector<float> current;
		for (int i = 0; i < n; i++)
			current.push_back(i * 1.f);


		// SETUP STATE 
		sim->setupNetwork();

		// setup monitoring
		std::vector<GroupMonitor*> gm;
		for_each(groups.begin(), groups.end(), [&](int g) {
			gm.push_back(sim->setGroupMonitor(g, "DEFAULT", 1));
			});

		std::vector<SpikeMonitor*> sm;
		for_each(groups.begin(), groups.end(), [&](int g) {
			sm.push_back(sim->setSpikeMonitor(g, "DEFAULT"));
			});

		// excite 
		for_each(groups.begin(), groups.end(), [&](int g) {
			sim->setExternalCurrent(g, current);
		});

		// RUN STATE 
		for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->startRecording(); });
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });

		sim->runNetwork(1, 0, true);

		for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->stopRecording(); });
		for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });
	
		for (int i = 0; i < sm.size(); i++) {
			SpikeMonitor* m = sm[i];
			//printf(" g%d mean firing rate: %.1f Hz\n", i, m->getPopMeanFiringRate());
			rates[mode][i] = m->getPopMeanFiringRate();
		}
		EXPECT_GT(rates[mode][1], 4.0f * rates[mode][0]);  // nm1 > control 
		EXPECT_GT(rates[mode][2], 5.0f * rates[mode][0]);  // nm1 > control 
		EXPECT_GT(rates[mode][3], 1.5f * rates[mode][0]);  // nm1 > control 

		delete sim;

		tested++;
	}

	// CPU vs GPU
	if (tested == 2) {  // first (CPU) is reference
		auto check = [&](int g) { EXPECT_NEAR(rates[0][g], rates[1][g], 0.5f); };
		for (int i = 1; i <= 3; i++)
			check(i);
	}

}

/* test projection
 * test w0->no activity, nm - ergic, target
 * test monitoring
 * test configuration 2 nms, release 
 * copy from NM4
 * 
 * target groups 
 * g_control
 * g_nm1
 * g_nm2
 * g_nm1nm2   e.g. DA, NE
 * 
 * gnm1ergic	nm1-ergic neuron group
 * gnm2ergic	nm2-ergic neuron group
 * gspike		spike generator
 * 
 * next: set IcalcType to M1R12M2R1, 
 * gin	spike gen same as I n but spike  -> second setup 
 */
TEST(Receptors, targets) {
	
	int n = 1;

	float baseNM = 0.001f; 
	float tauNM = 50;
	float releaseNM = 4.0f;
	int f = 100; // release frequency in Hz 

	float a = 0.02f, b = 0.2f, c = -55.0f, d = 4.0f; // IB

	int tested = 0;

	std::vector<float> data_nm1_DA[2];
	std::vector<float> data_nm2_NE[2];

	std::vector<float> data_nm12_DA[2];
	std::vector<float> data_nm12_NE[2];

	LoggerMode logger = SILENT; // SILENT USER DEVELOPER;
	for (int mode = 0; mode < TESTED_MODES; mode++) {
	//{int mode = 1;

	// first iteration, test CPU mode, second test GPU mode
	CARLsim* sim = new CARLsim("receptors_targets", mode ? GPU_MODE : CPU_MODE, logger, 1, 42);

	//nm-targets

	// control without nm projections
	int g_control = sim->createGroup("control", n, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);

	// nm1 DA
	int g_nm1 = sim->createGroup("nm1", n, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);

	// nm2 NE
	int g_nm2 = sim->createGroup("nm2", n, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);

	// nm12 DA,NE
	int g_nm12 = sim->createGroup("nm12", n, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);

	std::vector<int> groups = { g_control, g_nm1, g_nm2, g_nm12 };

	for_each(groups.begin(), groups.end(), [&](int g) {
		sim->setNeuronParameters(g, a, b, c, d); // IB
		sim->setNeuromodulator(g,
			baseNM, tauNM, releaseNM, true,
			0.001f, 1.f, 0.f, false,
			0.001f, 1.f, 0.f, false,
			baseNM, tauNM, releaseNM, true);
		});

	//nm-ergic

	// nm1 DA
	int g_nm1ergic = sim->createGroup("nm1ergic", 1, DOPAMINERGIC_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
	sim->setNeuronParameters(g_nm1ergic, 0.02f, 0.2f, -65.0f, 8.0f); // RS

	// nm2 NE
	int g_nm2ergic = sim->createGroup("nm2ergic", 1, NORADRENERGIC_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
	sim->setNeuronParameters(g_nm2ergic, 0.02f, 0.2f, -65.0f, 8.0f); // RS


	// w = 0 => do not trigger neural activity directly
	sim->connect(g_nm1ergic, g_nm1, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
	sim->connect(g_nm2ergic, g_nm2, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
	sim->connect(g_nm1ergic, g_nm12, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
	sim->connect(g_nm2ergic, g_nm12, "full", RangeWeight(.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);


	// CUBA
	sim->setConductances(false);

	// use periodic spike generator to know the exact dopamine delivery
	// 	cortex downstream activation
	int g_ctx = sim->createSpikeGeneratorGroup("ctx", 1, EXCITATORY_NEURON);  // 10*0.4 = 1.4  // ???
	// \todo  where is the 0.04f cofigured?

	sim->connect(g_ctx, g_nm1ergic, "full", RangeWeight(100.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // shall trigger exactly on spike and therefore NM release
	sim->connect(g_ctx, g_nm2ergic, "full", RangeWeight(100.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // shall trigger exactly on spike and therefore NM release

	// sensory input
	PeriodicSpikeGenerator* spkGen = new PeriodicSpikeGenerator(f, true);
	sim->setSpikeGenerator(g_ctx, spkGen);

	// SETUP STATE 
	sim->setupNetwork();

	// setup monitoring
	std::vector<GroupMonitor*> gm;
	for_each(groups.begin(), groups.end(), [&](int g) {		
		gm.push_back(sim->setGroupMonitor(g, "DEFAULT", 1));
		});

	std::vector<SpikeMonitor*> sm;
	for_each(groups.begin(), groups.end(), [&](int g) {
		sm.push_back(sim->setSpikeMonitor(g, "DEFAULT"));
		});


	// RUN STATE 
	for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->startRecording(); });
	for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->startRecording(); });

	sim->runNetwork(1, 0, true);
	sim->runNetwork(5, 0, true);

	for_each(gm.begin(), gm.end(), [&](GroupMonitor* m) {m->stopRecording(); });
	for_each(sm.begin(), sm.end(), [&](SpikeMonitor* m) {m->stopRecording(); });


	data_nm1_DA[mode] = gm[1]->getDataVector(NM_DA);  // g_nm1
	data_nm2_NE[mode] = gm[2]->getDataVector(NM_NE);  // g_nm2

	data_nm12_DA[mode] = gm[3]->getDataVector(NM_DA);  // g_nm1
	data_nm12_NE[mode] = gm[3]->getDataVector(NM_NE);  // g_nm2


	for (int i = 0; i < (data_nm1_DA[mode]).size(); i++) {
		//printf("nm1.DA:%.1f nm1.NE:%.1f  nm12.DA:%.1f nm12.NE:%.1f\n", data_nm1_DA[mode][i], data_nm2_NE[mode][i], data_nm12_DA[mode][i], data_nm12_NE[mode][i]);

		EXPECT_EQ((data_nm1_DA[mode])[i], (data_nm2_NE[mode])[i]);
		EXPECT_EQ(data_nm12_DA[mode][i], data_nm12_NE[mode][i]);
		EXPECT_EQ(data_nm1_DA[mode][i], data_nm12_DA[mode][i]);
		EXPECT_EQ(data_nm2_NE[mode][i], data_nm12_NE[mode][i]);
	}
	//auto data = m->getDataVector(NM_DA);
	auto& data = data_nm1_DA;

	delete spkGen;
	delete sim;
	}

	// CPU vs GPU
	if (tested == 2) {  // first (CPU) is reference
		for (int i = 0; i < data_nm1_DA[0].size(); i++) {
			EXPECT_EQ(data_nm1_DA[0][i], data_nm1_DA[1][i]);
			EXPECT_EQ(data_nm2_NE[0][i], data_nm2_NE[1][i]);
			EXPECT_EQ(data_nm12_DA[0][i], data_nm12_DA[1][i]);
			EXPECT_EQ(data_nm12_NE[0][i], data_nm12_NE[1][i]);
		}
	}

}

/*
*   
 * test tonic to phasic, bursting/chattering
 * with monitoring
 
	>> cd results
	>> NM = NetworkMonitor('./sim_receptors.oat.dat')
	>> NM.plot([1], 300);
*/
TEST(Receptors, phasic) {

	int n = 30;	// neurons, see also 40/400
	int ms = 300; // simulation length in ms

	LoggerMode logger = SILENT; // USER DEVELOPER;

	int tested = 0;
	float rates[TESTED_MODES][5];

	for (int mode = 0; mode < TESTED_MODES; mode++) {
	//{int mode = 0;

		CARLsim* sim = new CARLsim("receptors_phasic", mode ? GPU_MODE : CPU_MODE, logger, 1, 42);   // .phasic

		int g0 = sim->createGroup("g0_sink", 1, EXCITATORY_NEURON);
		int g1 = sim->createGroup("g1_rs", n, EXCITATORY_NEURON);
		int g2 = sim->createGroup("g2_ib", n, EXCITATORY_NEURON);
		int g3 = sim->createGroup("g3_ch", n, EXCITATORY_NEURON);
		int g4 = sim->createGroup("g4_ln", n, EXCITATORY_NEURON);

		sim->setNeuronParameters(g0, 0.02f, 0.2f, -65.0f, 8.0f); // RS  (sink)
		sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f); // RS  (control)
		sim->setNeuronParameters(g2, 0.02f, 0.2f, -55.0f, 4.0f); // IB
		sim->setNeuronParameters(g3, 0.02f, 0.2f, -50.0f, 2.0f); // CH
		sim->setNeuronParameters(g4, 0.02f, 0.2f, -52.5f, 3.0f); // LN1 initial idea  
		//sim->setNeuronParameters(g4, 0.02f, 0.2f, -52.0f, 2.7f); // LN2 30/300 optimal with clear cutover
		//sim->setNeuronParameters(g4, 0.02f, 0.2f, -52.5f, 3.5f); // LN3 40/400 optimal with clear cutover

		sim->setConductances(false);

		sim->connect(g1, g0, "random", RangeWeight(100.0), 0.1f);
		sim->connect(g2, g0, "random", RangeWeight(100.0), 0.1f);
		sim->connect(g3, g0, "random", RangeWeight(100.0), 0.1f);
		sim->connect(g4, g0, "random", RangeWeight(100.0), 0.1f);


		sim->setupNetwork();

	#ifdef SPK_MON
		SpikeMonitor* spkMonG1 = sim->setSpikeMonitor(g1, "DEFAULT");
		SpikeMonitor* spkMonG2 = sim->setSpikeMonitor(g2, "DEFAULT");
		SpikeMonitor* spkMonG3 = sim->setSpikeMonitor(g3, "DEFAULT");
		SpikeMonitor* spkMonG4 = sim->setSpikeMonitor(g4, "DEFAULT");
	#endif


		std::vector<float> current;

		for (int i = 0; i < n; i++)
			current.push_back(i * 1.f);


	#ifdef SPK_MON
		spkMonG1->startRecording();
		spkMonG2->startRecording();
		spkMonG3->startRecording();
		spkMonG4->startRecording();
	#endif

		sim->setExternalCurrent(g1, current);
		sim->setExternalCurrent(g2, current);
		sim->setExternalCurrent(g3, current);
		sim->setExternalCurrent(g4, current);

		sim->runNetwork(0, ms, true);

	#ifdef SPK_MON
		spkMonG1->stopRecording();
		spkMonG2->stopRecording();
		spkMonG3->stopRecording();
		spkMonG4->stopRecording();
	#endif


	#ifdef SPK_MON
		//printf("I_ext: %.0f  \n", i_ext);
		//printf(" g%d mean firing rate: %.1f Hz\n", g1, spkMonG1->getPopMeanFiringRate());
		//printf(" g%d mean firing rate: %.1f Hz\n", g2, spkMonG2->getPopMeanFiringRate());
		//printf(" g%d mean firing rate: %.1f Hz\n", g3, spkMonG3->getPopMeanFiringRate());
		//printf(" g%d mean firing rate: %.1f Hz\n", g4, spkMonG4->getPopMeanFiringRate());
		//printf(" g%d (sink) mean f. r: %.1f Hz\n", g0, spkMonG0->getPopMeanFiringRate());

		//EXPECT_FLOAT_EQ(spkMonG0->getPopMeanFiringRate(), inputRate); // sanity check
		//EXPECT_FLOAT_EQ(spkMonG1->getPopMeanFiringRate(), outputRate); // output
		//EXPECT_FLOAT_EQ(spkMonG2->getPopMeanFiringRate(), outputRate); // output

		auto saveRate = [&](int g, SpikeMonitor* spkMon) {
			//printf(" g%d mean firing rate: %.1f Hz\n", g, spkMon->getPopMeanFiringRate());
			rates[mode][g] = spkMon->getPopMeanFiringRate();
		};
		saveRate(g1, spkMonG1);
		saveRate(g2, spkMonG2);
		saveRate(g3, spkMonG3);
		saveRate(g4, spkMonG4);

	#endif

		// memory management
		delete sim;

		tested++;
	}

	// CPU vs GPU
	if (tested == 2) {  // first (CPU) is reference
		auto check = [&](int g) { EXPECT_NEAR(rates[0][g], rates[1][g], 0.5f); };
		for (int i = 1; i <= 4; i++)
			check(i);
	}
}


#endif
