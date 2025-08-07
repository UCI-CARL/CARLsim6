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



// compare monitor to deprecated method
// CARLsimGUI see EXPERIMENTAL_COBA_MON
TEST(PerfMon, partition) {

	double rate = rand() % 20 + 2.0;  // some random mean firing rate
	int isi = 1000 / rate; // inter-spike interval

	const int GroupSizes[] = { 100, 400, 1000, 4000, 10000, 40000, 100000 };


	int Delays[] = { 1,2,5,10,20 };
	int Neurons[] = { 100, 400, 1000, 4000, 10000, 40000, 100000 };

	int d_i = 2;  // 5 ms ENUM  d5, d20
	int N_i = 5;  // 100 ms   ENUM  N100, N400, 


	// chain
	int ms = 100; // ms 
	int d = Delays[d_i]; // 5 ms 
	int columns = ms / d;

	int N = Neurons[N_i]; // 100 
	int rows = N / columns;


	CARLsim* sim = new CARLsim("PerfMon.partition", CPU_MODE, USER, 0, 42);
	//CARLsim* sim = new CARLsim("PerfMon.slice", GPU_MODE, SILENT, 0, 42);

	sim->setConductances(true);



	const int N_exc = 4;
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
		g_exc[i] = sim->createGroup(name, N, EXCITATORY_NEURON, i+1);  // core 1..n for exc cluster
		sim->setNeuronParameters(g_exc[i], 0.02f, 0.2f, -65.0f, 8.0f);
	}

	for (int i = 0; i < N_exc+1; i++) {
		sprintf_s<length>(name, "g_inter%i", i);
		g_inter[i] = sim->createGroup(name, 1, EXCITATORY_NEURON, 0);  // core 0 is common base
		sim->setNeuronParameters(g_inter[i], 0.02f, 0.2f, -65.0f, 8.0f);
	}

	g_stim = sim->createSpikeGeneratorGroup("g_stim", 1, EXCITATORY_NEURON, 0);

	// TODO use Gene's stuff !!!
	
	// workaround with random
	for (int i = 0; i < N_exc; i++) {
		// c_d5
		//double p = (100 / columns) / (N * N); 
		float p = N / (N * N);    // 1/N -> each neuron is linear connected, however, it might not transport the spike 
		sim->connect(g_exc[i], g_exc[i], "random", RangeWeight(4.0), 0.01, d );   // 
		// pre
		p = rows / N; 
		sim->connect(g_inter[i], g_exc[i], "random", RangeWeight(4.0), 0.1 );
		sim->connect(g_exc[i], g_inter[i+1], "random", RangeWeight(4.0), 0.1);
	}

	// stim
	sim->connect(g_stim, g_inter[0], "one-to-one", RangeWeight(4.0), 1.0f);

	// Spike monitors to validate the SNN neural activity

	// Performance monitors 
	PerformanceMonitor* perfMon = sim->setPerformanceMonitor(PMB_INTEL, "DEFAULT");
	const int sample_rate = 1;
	perfMon->setSampleRate(sample_rate);


	// use periodic spike generator to know the exact spike times
	PeriodicSpikeGenerator spkGen(rate);
	sim->setSpikeGenerator(g_stim, &spkGen);

	//

	sim->setupNetwork();

	EXPECT_FALSE(perfMon->getPersistentData());	

	int nCores = 16;
	bool bPerfMon = true; 

	// CPU 3.5 s  3500
	// GPU 100,200,400,800 
	//const int slice = 100;
	const int slice = ms;  // ms
	for (int t = 0; t < 100; t += slice) {   // we do expect 4 x 100ms load on cores 1..4
 
		if(bPerfMon) perfMon->startRecording();
		sim->runNetwork(0, slice, true);
		if (bPerfMon) perfMon->stopRecording();

		if (bPerfMon) {
			auto lastUpdated = perfMon->getLastUpdated();
			EXPECT_EQ(lastUpdated, t + slice);

			auto util = perfMon->getUtilization();
			//EXPECT_EQ(util[0].size(), (t + slice) / 10);
			EXPECT_EQ(util[0].size(), ms / sample_rate);

			for (int coreIndex = 0; coreIndex < nCores; coreIndex++) {
				// EXPECT_GE(..)
				//	EXPECT_NEAR(util[0][slice - 1], util[nId], 0.001);
			}
		}

	}


	delete sim;
}

