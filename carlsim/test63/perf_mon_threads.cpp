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


#include <snn_definitions.h>

#define NUM_CPU_CORES 4

// compare monitor to deprecated method
// CARLsimGUI see EXPERIMENTAL_COBA_MON
TEST(PerfMon, threads) {

	double rate = rand() % 20 + 2.0;  // some random mean firing rate
	int isi = 1000 / rate; // inter-spike interval

	//const int N = 128; // 5 -  128;
	//const int N = 1200; // 5 -  128;    // Neurons per Partion
	const int N = 2400; // 5 -  128;    // Neurons per Partion, Reference test suite: Total = 2457600  x2, 10s
	//const int N = 4800; // 5 -  128;    // Neurons per Partion

	// basic idea:  per partion one thread !  = netId 
	// pin to CPU is _one_
	// however, to really compare, the SIZE must be increased 
	const int CPU_CORES = 4;
	const int PART_PER_CORE = 1;

	//const int CPU_CORES = 1; // 1 Thread
	//const int PART_PER_CORE = 4;  // 4x sized Network

	//const int CPU_CORES = 2; 
	//const int PART_PER_CORE = 2;


	//const int AFFINITY = 4;

	//CARLsim* sim = new CARLsim("PerfMon.threads", CPU_MODE, USER, 0, 42);  // DEVELOPER --> USER --> SILENT --> SHOWTIME
	CARLsim* sim = new CARLsim("CbMon.pthreads", GPU_MODE, SILENT, 0, 42);


	sim->setConductances(true);

	//	int g0 = sim->createSpikeGeneratorGroup("Input", N, EXCITATORY_NEURON);
	// 
	// use periodic spike generator to know the exact spike times
	PeriodicSpikeGenerator spkGenG0(rate);
	//	sim->setSpikeGenerator(g0, &spkGenG0);

	std::vector<int> g0(CPU_CORES);


	std::vector<int> g1(CPU_CORES);
	std::vector<int> g2(CPU_CORES);
	//std::vector<CobaMonitor*>  cbMon(CPU_CORES);

	for (int i = 0; i < CPU_CORES; i++) {

		std::string grpName;

		grpName = "g0_" + i;
		g0[i] = sim->createSpikeGeneratorGroup(grpName.c_str(), N / CPU_CORES, EXCITATORY_NEURON, i);
		sim->setSpikeGenerator(g0[i], &spkGenG0);  // CAUTION , shared !!!

		grpName = "g1_" + i;
		g1[i] = sim->createGroup(grpName.c_str(), N / CPU_CORES, EXCITATORY_NEURON, i);
		sim->setNeuronParameters(g1[i], 0.02f, 0.2f, -65.0f, 8.0f);

		grpName = "g2_" + i;
		g2[i] = sim->createGroup(grpName, N / CPU_CORES, INHIBITORY_NEURON, i);
		sim->setNeuronParameters(g2[i], 0.01f, 0.2f, -58.0f, 4.0f); // some pramassetCobaMonitor


		// synfire FFI 
		sim->connect(g0[i], g1[i], "one-to-one", RangeWeight(4.0), 0.5f);

		sim->connect(g0[i], g2[i], "one-to-one", RangeWeight(4.0), 0.5f, 3);

		sim->connect(g2[i], g1[i], "one-to-one", RangeWeight(0.05), 0.5f, 5);

	}

	PerformanceMonitor* perfMon = sim->setPerformanceMonitor(PMB_INTEL, "DEFAULT");
	perfMon->setSampleRate(10);


	sim->setupNetwork();

	

	int nCores = 12;  // Monitoring
	bool bPerfMon = true;

	// CPU 3.5 s  3500
	// GPU 100,200,400,800 
	const int slice = 100;
	//const int T = 500;
	const int T = 3500;
	//const int T = 14000;
	//const int T = 35000;
	for (int t = 0; t < T; t += slice) {

		if (bPerfMon) perfMon->startRecording();
		sim->runNetwork(0, slice, true);
		if (bPerfMon) perfMon->stopRecording();

		if (bPerfMon) {
			auto lastUpdated = perfMon->getLastUpdated();
			EXPECT_EQ(lastUpdated, t + slice);

			auto pdhUtil = perfMon->getUtilization();
			EXPECT_EQ(pdhUtil[0].size(), slice / 10);  // 10ms time step

			for (int coreIndex = 0; coreIndex < nCores; coreIndex++) {
				// EXPECT_GE(..)
				//	EXPECT_NEAR(util[0][slice - 1], util[nId], 0.001);
			}
		}

	}
	delete sim;
}