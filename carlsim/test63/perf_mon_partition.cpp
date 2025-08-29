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
#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>
#include <periodic_spikegen.h>
#include <snn_definitions.h> // MAX_GRP_PER_SNN

#include <cmath>
#include <vector>
#include <array>

typedef std::map<std::tuple<int, int>, uint8_t> delay_map_t;

class PartitionConnGen : public ConnectionGenerator
{
public:
	delay_map_t map;
	float w; 

	virtual void connect(CARLsim* s,
		int srcGrpId, int i,	// pre
		int destGrpId, int j,	// post
		float& weight,
		float& maxWt,
		float& delay,
		bool& connected) {

		try {
			delay = map.at(std::tuple<int, int>(i, j));
			weight = w;
			maxWt = 0.4f;
			connected = true;
		}
		//Exception thrown at : Microsoft C++ exception: std::out_of_range at 
		//catch (std::out_of_range) {
		catch (const std::out_of_range& ex) {
			delay = 0.f;
			connected = false;
		}
	}
};

class FanInConnGen : public PartitionConnGen
{
public:
	FanInConnGen(int N, int columns, int rows, int d = 20) {
		for (int post = 0; post < N; post++)
			if (post % columns == 0)    // first neuron of each chain (start)
				map.emplace(std::tuple<int, int>(0, post), d);
	}
};

class FanOutConnGen : public PartitionConnGen
{
public:
	FanOutConnGen(int N, int columns, int rows, int d = 20) {
		for (int pre = 0; pre < N; pre++)
			if (pre % columns == columns - 1)   // first neuron of each chain (start)
				map.emplace(std::tuple<int, int>(pre, 0), d);
	}
};

class ChainConnGen : public PartitionConnGen
{
public:
	ChainConnGen(int N, int columns, int rows, int d ) {
		for (int pre = 0; pre < N; pre++)
			for (int post = 0; post < N; post++)
			{
				int pre_r = pre / columns;
				int pre_c = pre % columns;
				int post_r = post / columns;
				int post_c = post % columns;

				// is chain element, order, length
				if (pre_r == post_r && post_c == pre_c + 1 && post_c < columns) {					
					map.emplace(std::tuple<int, int>(pre, post), d);
					//printf("pre: %2d  r: %2d c: %2d  -->  post: %2d r:%2d  c:%2d\n",
					//	pre, pre_r, pre_c, post, post_r, post_c);
				}
			}
	}
};


// compare monitor to deprecated method
// CARLsimGUI see EXPERIMENTAL_COBA_MON
TEST(PerfMon, partition) {

	bool bPerfMon = false; 
	//const int sample_rate = 1; // huge impact
	//const int sample_rate = 5; // moderate impact
	//const int sample_rate = 10; // moderate impact
	//const int sample_rate = 25; // no impact 
	//const int sample_rate = 50; // no impact 
	const int sample_rate = 100; // no impact  
	bool bSpikeMon = false;

	double rate = 1; // Hz
	int isi = 1000 / rate; // inter-spike interval, e.g. 500ms at 2 Hz

	int Delays[] = { 1,2,5,10,20 };
	int Neurons[] = { 20, 40, 100, 200, 400, 1000, 2000, 4000, 5000, 10000, 20000 };
	// 1k
	//int Neurons[] = { 20, 40, 100, 200, 400, 500, 1000, 2000, 4000, 5000, 10000, 20000 };

	// alles mit Debug version ->  Argumentation:  need sufficient load, determined,  w/o compiler optimizations
	// if released, the cores can be reduced e.g. to 2 in the example above   then speed factor 4.2  (vs. )
	// = core / but this is due to small workload on an i9

	int d_i = 2;  // 5 ms ENUM  d5, d20   // -->  2.9 !!!  => rule of thumb x 2 partitions & <= max(phys. cores)  !!! ++ affinity to avoid context switching, _ cache line invalidated _
	//int d_i = 3;  // 10 ms ENUM  d5, d20
	
	//int N_i = 5;  // 100 ms   ENUM  N100, N400,     AB 5 kommt neues Bottle-Neck hinzu!!! => Workload zu klein, Parall. nicht mehr mgl
	int N_i = CARLsim::Params()[CUSTOM_2_PARAM];

	// chain
	int ms = 100; // ms 
	int d = Delays[d_i]; // 5 ms 
	int columns = ms / d;

	int N = Neurons[N_i]; // 100 
	int rows = N / columns;


	// see main.cpp
	//CARLsim::InitParams("carlsim.conf");
	//CARLsim::InitParams(); // Env.
	//xx

	// Overwrites
	//CARLsim::Params()[LOGGER_MODE_PARAM] = USER;

	//CARLsim::Params()[RAND_SEED_PARAM] = 40;  // trigger exception

	//CARLsim::Params()[CARLSIM_LOGGER_MODE] = USER;

	//CARLsim::NParams = 4;
	//auto devices = CARLsim::cudaDeviceCount();


	CARLsim* sim = new CARLsim("PerfMon.partition", CPU_MODE, SILENT, 0, 42);
	//CARLsim* sim = new CARLsim("PerfMon.partition", CPU_MODE, USER, 0, 42);
	//CARLsim* sim = new CARLsim("PerfMon.partition", GPU_MODE, USER, 0, 42);

	sim->setConductances(true);

	//sim->setIntegrationMethod(FORWARD_EULER, 2);
	//sim->setIntegrationMethod(FORWARD_EULER, 4);  // CARLsim default ?!
	//sim->setIntegrationMethod(FORWARD_EULER, 10);
 	//sim->setIntegrationMethod(FORWARD_EULER, 20);   // same results
	//sim->setIntegrationMethod(FORWARD_EULER, 40);

	//sim->setIntegrationMethod(RUNGE_KUTTA4, 4);
	sim->setIntegrationMethod(RUNGE_KUTTA4, 10);  // recomended for Izh9 or Compartment   => !!! Here the small network of 1000 neurons does not run at realtime despite the availabi cores !!!! --->  this is the very ARGUMENT for edge, smart phone 
	//sim->setIntegrationMethod(RUNGE_KUTTA4, 20);  // 21 s (single core)
	//sim->setIntegrationMethod(RUNGE_KUTTA4, 40);  // 45 s (single core), 60 s (multi core)
	//sim->setIntegrationMethod(RUNGE_KUTTA4, 100); // 113 (multi core)


	//int nCores = 16;   // 80% --> 2.4x
	//int nCores = 8;    // 83.2% -> 2.9x     !!! 16 -> 2.4x   !!! 32 --> 96%  AND >95% BLOCKING    4 -> 2.3 (prob. most power eff) / partitions / 2  --> 1.4

	//const int N_exc = 1;   // Release    4.5
	const int N_exc = 2;   // Release    4.5   // Results presented Jeff Fr. 29.08.2025
	//const int N_exc = 4;   // Release    4.5
	//const int N_exc = 8;   // Release    4.5
	//const int N_exc = CARLsim::Params()[CUSTOM_2_PARAM];   // Release    4.5

	/*
	ST
	********************Simulation Summary***************************
		Network Parameters : numNeurons = 2004 (numNExcReg : numNInhReg = 100.0 : 0.0)
		numSynapses = 2101
		maxDelay = 20
		Simulation Mode : COBA
		Random Seed : 42
		Timing : Model Simulation Time = 10 sec
		Actual Execution Time = 8.84 sec
		Speed Factor(Model / Real) = 1.1 x		           => barely made it
		Average Firing Rate : 2 + ms delay = 1.000 Hz
		1ms delay = 1.000 Hz
		Overall = 1.000 Hz
		Overall Spike Count Transferred :
	2 + ms delay = 10010
		1ms delay = 0
		Overall Spike Count : 2 + ms delay = 20030
		1ms delay = 10
		Total = 20040

vs. safely done
	omp_set_dynamic(0);  // 1 20s
	//omp_set_num_threads(1);  // 1 working for omp parallel for  -> this produces the .. fixed load on n cores
	omp_set_num_threads(2);

Timing:                 Model Simulation Time = 10 sec
						Actual Execution Time = 5.10 sec
						Speed Factor (Model/Real) = 2.0 x
Average Firing Rate:    2+ms delay = 1.000 Hz
						1ms delay = 1.000 Hz
						Overall = 1.000 Hz
Overall Spike Count Transferred:
						2+ms delay = 10010
						1ms delay = 0
Overall Spike Count:    2+ms delay = 20030
						1ms delay = 10
						Total = 20040


with 4 --> 3.3x !!!
with 8 --> 4.5x 
with 16 --> 4.0x  --> spin !!!  (kernel)

d.h. 2000 neurons = x2 sicherheit
x4.5 sicherheit ggü. RT --> learning or sparse processing --> see slow down
		*/

	//const int N_exc = 4;  // Debug
	//const int N_exc = 8;
	//const int N_exc = 16;
	//const int N_exc = 1;
	int g_exc[N_exc];
	//int conn_exc[N_exc];
	int g_inter[N_exc+1];   
	//int conn_pre[N_exc];
	//int conn_post[N_exc];
	int g_stim;


	const size_t length = 100; 
	char name[length];
	for (int i = 0; i < N_exc; i++) {
		sprintf_s<length>(name, "g_exc%i", i);
		g_exc[i] = sim->createGroup(name, N, EXCITATORY_NEURON, i);  // core 1..n for exc cluster
		//g_exc[i] = sim->createGroup(name, N, EXCITATORY_NEURON, 0);  // core 1..n for exc cluster
		//g_exc[i] = sim->createGroup(name, N, EXCITATORY_NEURON, 0, GPU_CORES);  // core 1..n for exc cluster
		sim->setNeuronParameters(g_exc[i], 0.02f, 0.2f, -65.0f, 8.0f);
	}

	for (int i = 0; i < N_exc+1; i++) {
		sprintf_s<length>(name, "g_inter%i", i);
		g_inter[i] = sim->createGroup(name, 1, EXCITATORY_NEURON, 0);  // core 0 is common base
		sim->setNeuronParameters(g_inter[i], 0.02f, 0.2f, -65.0f, 8.0f);
	}

	g_stim = sim->createSpikeGeneratorGroup("g_stim", 1, EXCITATORY_NEURON, 0);


	// stim
	sim->connect(g_stim, g_inter[0], "one-to-one", RangeWeight(0.3), 1.0f, 10);

	// Offer TestCase to userconn
	FanInConnGen in(N, columns, rows, 10);			in.w	= 0.3;
	ChainConnGen chain(N, columns, rows, d);	chain.w = 0.325;
	FanOutConnGen out(N, columns, rows, 10);  		out.w = 0.3 / rows;
	//ChainConnGen chain(N, columns, rows, d);	chain.w = 0.375;

	for (int i = 0; i < N_exc; i++) {
		sim->connect(g_inter[i], g_exc[i], &in, SYN_FIXED);
		sim->connect(g_exc[i], g_exc[i], &chain, SYN_FIXED);
		sim->connect(g_exc[i], g_inter[i + 1], &out, SYN_FIXED);
	}

	//// random connection
	//for (int i = 0; i < N_exc; i++) {
	//	// c_d5
	//	//double p = (100 / columns) / (N * N); 
	//	float p = N / (N * N);    // 1/N -> each neuron is linear connected, however, it might not transport the spike 
	//	sim->connect(g_exc[i], g_exc[i], "random", RangeWeight(4.0), 0.01, d );   // 
	//	// pre
	//	p = rows / N; 
	//	sim->connect(g_inter[i], g_exc[i], "random", RangeWeight(4.0), 0.1 );
	//	sim->connect(g_exc[i], g_inter[i+1], "random", RangeWeight(4.0), 0.1);
	//}

	std::vector<SpikeMonitor*> excSpikeMon(N_exc);
	std::vector<SpikeMonitor*> interSpikeMon(N_exc+1);
	if (bSpikeMon) {
		// Spike monitors to validate the SNN neural activity	
		for (int i = 0; i < N_exc; i++)
			excSpikeMon[i] = sim->setSpikeMonitor(g_exc[i], "DEFAULT");
		for (int i = 0; i < N_exc + 1; i++)
			interSpikeMon[i] = sim->setSpikeMonitor(g_inter[i], "DEFAULT");
	}

	PerformanceMonitor* perfMon = nullptr;
	if (bPerfMon) {
		// Performance monitors 
		perfMon = sim->setPerformanceMonitor(PMB_INTEL, "DEFAULT");
		//perfMon = sim->setPerformanceMonitor(PMB_INTEL, "NULL");
		perfMon->setSampleRate(sample_rate);
	}

	// use periodic spike generator to know the exact spike times
	PeriodicSpikeGenerator spkGen(rate, true);  // 2 Hz => ISI 500 ms
	sim->setSpikeGenerator(g_stim, &spkGen);

	//

	sim->setupNetwork();

//	EXPECT_FALSE(perfMon->getPersistentData());	


	// CPU 3.5 s  3500
	// GPU 100,200,400,800 
	//const int slice = 100;
	const int slice = ms;  // ms

	// does the param work together with gtest ?see main...
	// do implement as CARLsim::static
	// carlsimParam,  carlsimCmd, carlsimConfFile, carlsimEnvVars
	int t_model = CARLsim::Params()[CUSTOM_1_PARAM]; // CARLSIM_CUSTOM_1 = 10000
	if (t_model == -1)
		t_model = 100; // ms

	for (int t = 0; t < 500 * 2 * t_model; t += slice) {   // we do expect 4 x 100ms load on cores 1..4
 
		if(bPerfMon) 
			perfMon->startRecording();
		if (bSpikeMon) {
			for (int i = 0; i < N_exc; i++)
				excSpikeMon[i]->startRecording();
			for (int i = 0; i < N_exc+1; i++)
				interSpikeMon[i]->startRecording();
		}

		sim->runNetwork(0, slice, false);

		if(bPerfMon) 
			perfMon->stopRecording();
		if (bSpikeMon) {
			for (int i = 0; i < N_exc; i++)
				excSpikeMon[i]->stopRecording();
			for (int i = 0; i < N_exc+1; i++)
				interSpikeMon[i]->stopRecording();
		}


		if (bPerfMon) {
			//auto lastUpdated = perfMon->getLastUpdated();
			//EXPECT_EQ(lastUpdated, t + slice);

			//auto util = perfMon->getUtilization();
			////EXPECT_EQ(util[0].size(), (t + slice) / 10);
			//EXPECT_EQ(util[0].size(), ms / sample_rate);

			//for (int coreIndex = 0; coreIndex < nCores; coreIndex++) {
			//	// EXPECT_GE(..)
			//	//	EXPECT_NEAR(util[0][slice - 1], util[nId], 0.001);
			//}
		}

	}


	delete sim;
}

