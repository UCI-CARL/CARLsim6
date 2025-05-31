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


TEST(CbMon, file) {

	double rate = rand() % 20 + 2.0;  // some random mean firing rate
	int isi = 1000 / rate; // inter-spike interval



	CARLsim* sim = new CARLsim("CbMon.file", CPU_MODE, SILENT, 1, 42);

	sim->setConductances(true);

	int g1 = sim->createGroup("g1", 1, EXCITATORY_NEURON);
	sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);

	int g0 = sim->createSpikeGeneratorGroup("Input", 5, EXCITATORY_NEURON);

	//NeuronMonitor* nrnMon = sim->setNeuronMonitor(g1, "DEFAULT");

	CobaMonitor* cbMon = sim->setCobaMonitor(g1, "DEFAULT");

	sim->connect(g0, g1, "random", RangeWeight(4.0), 0.5f);

	// use periodic spike generator to know the exact spike times
	PeriodicSpikeGenerator spkGenG0(rate);
	sim->setSpikeGenerator(g0, &spkGenG0);

	sim->setupNetwork();

	EXPECT_FALSE(cbMon->getPersistentData());

	cbMon->startRecording();
	sim->runNetwork(0, 10, true);
	cbMon->stopRecording();

	auto lastUpdated = cbMon->getLastUpdated();
	auto vectorAMPA = cbMon->getVectorAMPA();
	EXPECT_EQ(lastUpdated, 10);
	EXPECT_EQ(vectorAMPA [0] .size(), 10);

	cbMon->startRecording();
	sim->runNetwork(0, 10, true);
	cbMon->stopRecording();

	lastUpdated = cbMon->getLastUpdated();
	vectorAMPA = cbMon->getVectorAMPA();
	EXPECT_EQ(lastUpdated, 20);
	EXPECT_EQ(vectorAMPA[0].size(), 10);


	// now switch persistent mode on, buffer is extended
	cbMon->setPersistentData(true);

	EXPECT_TRUE(cbMon->getPersistentData());

	cbMon->startRecording();
	sim->runNetwork(0, 10, true);
	cbMon->stopRecording();

	lastUpdated = cbMon->getLastUpdated();
	vectorAMPA = cbMon->getVectorAMPA();
	EXPECT_EQ(lastUpdated, 30);
	EXPECT_EQ(vectorAMPA[0].size(), 20);

	cbMon->startRecording();
	sim->runNetwork(0, 10, true);
	cbMon->stopRecording();

	lastUpdated = cbMon->getLastUpdated();
	vectorAMPA = cbMon->getVectorAMPA();
	EXPECT_EQ(lastUpdated, 40);
	EXPECT_EQ(vectorAMPA[0].size(), 30);



	delete sim;
}




// compare monitor to deprecated method
// CARLsimGUI see EXPERIMENTAL_COBA_MON
TEST(CbMon, compatibility) {

	double rate = rand() % 20 + 2.0;  // some random mean firing rate
	int isi = 1000 / rate; // inter-spike interval

	const int N = 5;

	CARLsim* sim = new CARLsim("CbMon.compat", CPU_MODE, SILENT, 0, 42);
	//CARLsim* sim = new CARLsim("CbMon.compat", GPU_MODE, SILENT, 0, 42);

	sim->setConductances(true);


	int g1 = sim->createGroup("g1", N, EXCITATORY_NEURON);
	sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);

	int g2 = sim->createGroup("g2", N, INHIBITORY_NEURON);
	sim->setNeuronParameters(g2, 0.01f, 0.2f, -58.0f, 4.0f); // some pramassetCobaMonitor

	int g0 = sim->createSpikeGeneratorGroup("Input", N, EXCITATORY_NEURON);

	//NeuronMonitor* nrnMon = sim->setNeuronMonitor(g1, "DEFAULT");

	CobaMonitor* cbMon = sim->setCobaMonitor(g1, "DEFAULT");

	// synfire FFI 
	sim->connect(g0, g1, "one-to-one", RangeWeight(4.0), 0.5f);

	sim->connect(g0, g2, "one-to-one", RangeWeight(4.0), 0.5f);

	sim->connect(g2, g1, "one-to-one", RangeWeight(2.0), 0.5f);

	// use periodic spike generator to know the exact spike times
	PeriodicSpikeGenerator spkGenG0(rate);
	sim->setSpikeGenerator(g0, &spkGenG0);

	sim->setupNetwork();

	EXPECT_FALSE(cbMon->getPersistentData());

	// CPU 3.5 s  3500
	// GPU 100,200,400,800 
	for (int t = 0; t < 3500; t++) {

		cbMon->startRecording();
		sim->runNetwork(0, 1, false);
		cbMon->stopRecording();
		
		auto lastUpdated = cbMon->getLastUpdated();
		EXPECT_EQ(lastUpdated, t+1);

		auto vectorAMPA = cbMon->getVectorAMPA();
		auto vectorNMDA = cbMon->getVectorNMDA();
		auto vectorGABAa = cbMon->getVectorGABAa();
		auto vectorGABAb = cbMon->getVectorGABAb();
	
		EXPECT_EQ(vectorAMPA.size(), N+1);  // mean
		EXPECT_EQ(vectorNMDA.size(), N+1);
		EXPECT_EQ(vectorGABAa.size(), N+1);
		EXPECT_EQ(vectorGABAb.size(), N+1);

		EXPECT_EQ(vectorAMPA[0].size(), 1);
		EXPECT_EQ(vectorNMDA[0].size(), 1);
		EXPECT_EQ(vectorGABAa[0].size(), 1);
		EXPECT_EQ(vectorGABAb[0].size(), 1);

		auto gGrpId = g1;
		// CARLsimGUI see EXPERIMENTAL_COBA_MON
		std::vector<float> ampa = sim->getConductanceAMPA(gGrpId);
		std::vector<float> nmda = sim->getConductanceNMDA(gGrpId);
		std::vector<float> gaba_a = sim->getConductanceGABAa(gGrpId);
		std::vector<float> gaba_b = sim->getConductanceGABAb(gGrpId);

		EXPECT_EQ(ampa.size(), N);
		EXPECT_EQ(nmda.size(), N);
		EXPECT_EQ(gaba_a.size(), N);
		EXPECT_EQ(gaba_b.size(), N);

		EXPECT_NEAR(vectorAMPA[0][0], ampa[0], 0.001);
		EXPECT_NEAR(vectorNMDA[0][0], nmda[0], 0.001);
		EXPECT_NEAR(vectorGABAa[0][0], gaba_a[0], 0.001);
		EXPECT_NEAR(vectorGABAb[0][0], gaba_b[0], 0.001);

	}

	delete sim;
}





// compare monitor to deprecated method
// CARLsimGUI see EXPERIMENTAL_COBA_MON
TEST(CbMon, slice) {

	double rate = rand() % 20 + 2.0;  // some random mean firing rate
	int isi = 1000 / rate; // inter-spike interval

	const int N = 128; // 5 -  128;

	CARLsim* sim = new CARLsim("CbMon.slice", CPU_MODE, SILENT, 0, 42);
	//CARLsim* sim = new CARLsim("CbMon.slice", GPU_MODE, SILENT, 0, 42);

	sim->setConductances(true);


	int g1 = sim->createGroup("g1", N, EXCITATORY_NEURON);
	sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);

	int g2 = sim->createGroup("g2", N, INHIBITORY_NEURON);
	sim->setNeuronParameters(g2, 0.01f, 0.2f, -58.0f, 4.0f); // some pramassetCobaMonitor

	int g0 = sim->createSpikeGeneratorGroup("Input", N, EXCITATORY_NEURON);

	//NeuronMonitor* nrnMon = sim->setNeuronMonitor(g1, "DEFAULT");

	CobaMonitor* cbMon = sim->setCobaMonitor(g1, "DEFAULT");

	// synfire FFI 
	sim->connect(g0, g1, "one-to-one", RangeWeight(4.0), 0.5f);

	sim->connect(g0, g2, "one-to-one", RangeWeight(4.0), 0.5f, 3);

	sim->connect(g2, g1, "one-to-one", RangeWeight(0.05), 0.5f, 5);

	// use periodic spike generator to know the exact spike times
	PeriodicSpikeGenerator spkGenG0(rate);
	sim->setSpikeGenerator(g0, &spkGenG0);

	sim->setupNetwork();

	EXPECT_FALSE(cbMon->getPersistentData());	

	// CPU 3.5 s  3500
	// GPU 100,200,400,800 
	const int slice = 100;
	//for (int t = 0; t < 350; t += slice) {
	for (int t = 0; t < 3500; t += slice) {
	//for (int t = 0; t < 3500*10; t += slice) {
 
		cbMon->startRecording();
		sim->runNetwork(0, slice, true);
		cbMon->stopRecording();

		auto lastUpdated = cbMon->getLastUpdated();
		EXPECT_EQ(lastUpdated, t + slice);

		auto vectorAMPA = cbMon->getVectorAMPA();
		auto vectorNMDA = cbMon->getVectorNMDA();
		auto vectorGABAa = cbMon->getVectorGABAa();
		auto vectorGABAb = cbMon->getVectorGABAb();

		EXPECT_EQ(vectorAMPA.size(), N + 1);  // mean
		EXPECT_EQ(vectorNMDA.size(), N + 1);
		EXPECT_EQ(vectorGABAa.size(), N + 1);
		EXPECT_EQ(vectorGABAb.size(), N + 1);

		EXPECT_EQ(vectorAMPA[0].size(), slice);
		EXPECT_EQ(vectorNMDA[0].size(), slice);
		EXPECT_EQ(vectorGABAa[0].size(), slice);
		EXPECT_EQ(vectorGABAb[0].size(), slice);

		auto gGrpId = g1;
		// CARLsimGUI see EXPERIMENTAL_COBA_MON
		std::vector<float> ampa = sim->getConductanceAMPA(gGrpId);
		std::vector<float> nmda = sim->getConductanceNMDA(gGrpId);
		std::vector<float> gaba_a = sim->getConductanceGABAa(gGrpId);
		std::vector<float> gaba_b = sim->getConductanceGABAb(gGrpId);

		EXPECT_EQ(ampa.size(), N);
		EXPECT_EQ(nmda.size(), N);
		EXPECT_EQ(gaba_a.size(), N);
		EXPECT_EQ(gaba_b.size(), N);

		EXPECT_NEAR(vectorAMPA[0][slice - 1], ampa[0], 0.001);
		EXPECT_NEAR(vectorNMDA[0][slice - 1], nmda[0], 0.001);
		EXPECT_NEAR(vectorGABAa[0][slice - 1], gaba_a[0], 0.001);
		EXPECT_NEAR(vectorGABAb[0][slice - 1], gaba_b[0], 0.001);

		for (int nId = 0; nId < N; nId++) {
			EXPECT_NEAR(vectorAMPA[0][slice - 1], ampa[nId], 0.001);
			EXPECT_NEAR(vectorNMDA[0][slice - 1], nmda[nId], 0.001);
			EXPECT_NEAR(vectorGABAa[0][slice - 1], gaba_a[nId], 0.001);
			EXPECT_NEAR(vectorGABAb[0][slice - 1], gaba_b[nId], 0.001);
		}
	}

	delete sim;
}



#include <snn_definitions.h>

#define NUM_CPU_CORES 4

// compare monitor to deprecated method
// CARLsimGUI see EXPERIMENTAL_COBA_MON
TEST(CbMon, pthreads) {

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

	CARLsim* sim = new CARLsim("CbMon.pthreads", CPU_MODE, SILENT, 0, 42);  // DEVELOPER --> USER --> SILENT
	//CARLsim* sim = new CARLsim("CbMon.pthreads", GPU_MODE, SILENT, 0, 42);


	sim->setConductances(true);

//	int g0 = sim->createSpikeGeneratorGroup("Input", N, EXCITATORY_NEURON);
// 
// use periodic spike generator to know the exact spike times
	PeriodicSpikeGenerator spkGenG0(rate);
//	sim->setSpikeGenerator(g0, &spkGenG0);

	std::vector<int> g0(CPU_CORES);


	std::vector<int> g1(CPU_CORES);
	std::vector<int> g2(CPU_CORES);
	std::vector<CobaMonitor*>  cbMon(CPU_CORES);

	for (int i = 0; i < CPU_CORES; i++) {

		std::string grpName;

		grpName = "g0_" + i;
		g0[i] = sim->createSpikeGeneratorGroup(grpName.c_str(), N/CPU_CORES, EXCITATORY_NEURON, i);
		sim->setSpikeGenerator(g0[i], &spkGenG0);  // CAUTION , shared !!!

		grpName = "g1_" + i;		
		g1[i] = sim->createGroup(grpName.c_str(), N/CPU_CORES, EXCITATORY_NEURON, i);
		sim->setNeuronParameters(g1[i], 0.02f, 0.2f, -65.0f, 8.0f);

		grpName = "g2_" + i;
		g2[i] = sim->createGroup(grpName, N / CPU_CORES, INHIBITORY_NEURON, i);
		sim->setNeuronParameters(g2[i], 0.01f, 0.2f, -58.0f, 4.0f); // some pramassetCobaMonitor

		////NeuronMonitor* nrnMon = sim->setNeuronMonitor(g1, "DEFAULT");

		//cbMon[i] = sim->setCobaMonitor(g1[i], "DEFAULT");

		// synfire FFI 
		sim->connect(g0[i], g1[i], "one-to-one", RangeWeight(4.0), 0.5f);

		sim->connect(g0[i], g2[i], "one-to-one", RangeWeight(4.0), 0.5f, 3);

		sim->connect(g2[i], g1[i], "one-to-one", RangeWeight(0.05), 0.5f, 5);

	}



	sim->setupNetwork();

	//EXPECT_FALSE(cbMon->getPersistentData());

	//// investigate pthread overhead: is it the instanciation or intrinsic during run?
	//sim->runNetwork(2, 0, true);  // sed * 1000 times call to the create thread x 4 num cores !!!
	//sim->runNetwork(10, 0, true);  // sed * 1000 times call to the create thread x 4 num cores !!!
	sim->runNetwork(20, 0, true);  // sed * 1000 times call to the create thread x 4 num cores !!!
	// solution: implement worker that are created once and sleep after each step
	// block by barrier 

	// CPU 3.5 s  3500
	//const int slice = 100;
	//const int slice = 200;
	//const int slice = 500;
	const int slice = 500;
	//const int slice = 10;  // hangs after ~20 iterations  race / deadlock / ???? 
	//for (int t = 0; t < 350; t += slice) {
//	for (int t = 0; t < 3000; t += slice) {
		//for (int t = 0; t < 3000*10; t += slice) {

		//cbMon->startRecording();
//		sim->runNetwork(0, slice, true);
		//cbMon->stopRecording();

		//auto lastUpdated = cbMon->getLastUpdated();
		//EXPECT_EQ(lastUpdated, t + slice);

		//auto vectorAMPA = cbMon->getVectorAMPA();
		//auto vectorNMDA = cbMon->getVectorNMDA();
		//auto vectorGABAa = cbMon->getVectorGABAa();
		//auto vectorGABAb = cbMon->getVectorGABAb();

		//EXPECT_EQ(vectorAMPA.size(), N + 1);  // mean
		//EXPECT_EQ(vectorNMDA.size(), N + 1);
		//EXPECT_EQ(vectorGABAa.size(), N + 1);
		//EXPECT_EQ(vectorGABAb.size(), N + 1);

		//EXPECT_EQ(vectorAMPA[0].size(), slice);
		//EXPECT_EQ(vectorNMDA[0].size(), slice);
		//EXPECT_EQ(vectorGABAa[0].size(), slice);
		//EXPECT_EQ(vectorGABAb[0].size(), slice);

		//auto gGrpId = g1;
		//// CARLsimGUI see EXPERIMENTAL_COBA_MON
		//std::vector<float> ampa = sim->getConductanceAMPA(gGrpId);
		//std::vector<float> nmda = sim->getConductanceNMDA(gGrpId);
		//std::vector<float> gaba_a = sim->getConductanceGABAa(gGrpId);
		//std::vector<float> gaba_b = sim->getConductanceGABAb(gGrpId);

		//EXPECT_EQ(ampa.size(), N);
		//EXPECT_EQ(nmda.size(), N);
		//EXPECT_EQ(gaba_a.size(), N);
		//EXPECT_EQ(gaba_b.size(), N);

		//EXPECT_NEAR(vectorAMPA[0][slice - 1], ampa[0], 0.001);
		//EXPECT_NEAR(vectorNMDA[0][slice - 1], nmda[0], 0.001);
		//EXPECT_NEAR(vectorGABAa[0][slice - 1], gaba_a[0], 0.001);
		//EXPECT_NEAR(vectorGABAb[0][slice - 1], gaba_b[0], 0.001);

		//for (int nId = 0; nId < N; nId++) {
		//	EXPECT_NEAR(vectorAMPA[0][slice - 1], ampa[nId], 0.001);
		//	EXPECT_NEAR(vectorNMDA[0][slice - 1], nmda[nId], 0.001);
		//	EXPECT_NEAR(vectorGABAa[0][slice - 1], gaba_a[nId], 0.001);
		//	EXPECT_NEAR(vectorGABAb[0][slice - 1], gaba_b[nId], 0.001);
		//}
	//}

	// TODO 
	// EXPECT 

/* NO_CPPTHREADS && NO_PTHREADS && NO_CUDA  

--gtest_filter=-XCbMon.pthreads

		********************Running the simulation on 0 GPU(s) and 4 CPU(s) * **************************

		********************Simulation Summary * **************************
		Network Parameters : numNeurons = 7200 (numNExcReg : numNInhReg = 33.3 : 33.3)
		numSynapses = 7200
		maxDelay = 5
		Simulation Mode : COBA
		Random Seed : 42
		Timing : Model Simulation Time = 20 sec
		Actual Execution Time = 9.35 sec
		Speed Factor(Model / Real) = 2.1 x(Debug)
		Average Firing Rate : 2 + ms delay = 73.763 Hz
		1ms delay = 56.450 Hz
		Overall = 67.992 Hz
		Overall Spike Count Transferred :
	2 + ms delay = 0
		1ms delay = 0
		Overall Spike Count : 2 + ms delay = 7081200
		1ms delay = 2709600
		Total = 9790800
		* ********************************************************************************				

		
--gtest_filter=CbMon.pthreads
		
******************** Running the simulation on 0 GPU(s) and 4 CPU(s) ***************************

********************    Simulation Summary      ***************************
Network Parameters:     numNeurons = 7200 (numNExcReg:numNInhReg = 33.3:33.3)
						numSynapses = 7200
						maxDelay = 5
Simulation Mode:        COBA
Random Seed:            42
Timing:                 Model Simulation Time = 20 sec
						Actual Execution Time = 8.14 sec
						Speed Factor (Model/Real) = 2.5 x (Debug)
Average Firing Rate:    2+ms delay = 16.562 Hz
						1ms delay = 18.075 Hz
						Overall = 17.067 Hz
Overall Spike Count Transferred:
						2+ms delay = 0
						1ms delay = 0
Overall Spike Count:    2+ms delay = 1590000
						1ms delay = 867600
						Total = 2457600
*********************************************************************************

		
*/

/* CPPTHREADS 

		********************Running the simulation on 0 GPU(s) and 4 CPU(s) * **************************

		Create pool of 4 threads with affinity to core[4], .. for SNN::doSTPUpdateAndDecayCond
		Create pool of 4 threads with affinity to core[4], .. for SNN::updateTimingTable
		Create pool of 4 threads with affinity to core[4], .. for SNN::doCurrentUpdateD2
		Create pool of 4 threads with affinity to core[4], .. for SNN::doCurrentUpdateD1
		Create pool of 4 threads with affinity to core[4], .. for SNN::globalStateUpdate
		Create pool of 4 threads with affinity to core[4], .. for SNN::clearExtFiringTable


		******************** Simulation Summary***************************
		Network Parameters : numNeurons = 7200 (numNExcReg : numNInhReg = 33.3 : 33.3)
		numSynapses = 7200
		maxDelay = 5
		Simulation Mode : COBA
		Random Seed : 42
		Timing : Model Simulation Time = 20 sec
		Actual Execution Time = 21.31 sec
		Speed Factor(Model / Real) = 93.9 % (Debug)
		Average Firing Rate : 2 + ms delay = 16.562 Hz
		1ms delay = 18.075 Hz
		Overall = 17.067 Hz
		Overall Spike Count Transferred :
	2 + ms delay = 0
		1ms delay = 0
		Overall Spike Count : 2 + ms delay = 1590000
		1ms delay = 867600
		Total = 2457600
		* ********************************************************************************

*/


/*  !defined(NO_CUDA)  with NO_CPPTHREADS && NO_PTHREADS   

--gtest_filter=-XCbMon.pthreads

******************** Running the simulation on 0 GPU(s) and 4 CPU(s) ***************************

********************    Simulation Summary      ***************************
Network Parameters:     numNeurons = 7200 (numNExcReg:numNInhReg = 33.3:33.3)
						numSynapses = 7200
						maxDelay = 5
Simulation Mode:        COBA
Random Seed:            42
Timing:                 Model Simulation Time = 20 sec
						Actual Execution Time = 9.67 sec
						Speed Factor (Model/Real) = 2.1 x (Debug)
Average Firing Rate:    2+ms delay = 73.763 Hz
						1ms delay = 56.450 Hz
						Overall = 67.992 Hz
Overall Spike Count Transferred:
						2+ms delay = 0
						1ms delay = 0
Overall Spike Count:    2+ms delay = 7081200
						1ms delay = 2709600
						Total = 9790800
*********************************************************************************

!!! YES it the static / missing memory mgt !!!

--gtest_filter=CbMon.pthreads

******************** Running the simulation on 0 GPU(s) and 4 CPU(s) ***************************

********************    Simulation Summary      ***************************
Network Parameters:     numNeurons = 7200 (numNExcReg:numNInhReg = 33.3:33.3)
						numSynapses = 7200
						maxDelay = 5
Simulation Mode:        COBA
Random Seed:            42
Timing:                 Model Simulation Time = 20 sec
						Actual Execution Time = 8.38 sec
						Speed Factor (Model/Real) = 2.4 x (Debug)
Average Firing Rate:    2+ms delay = 16.562 Hz
						1ms delay = 18.075 Hz
						Overall = 17.067 Hz
Overall Spike Count Transferred:
						2+ms delay = 0
						1ms delay = 0
Overall Spike Count:    2+ms delay = 1590000
						1ms delay = 867600
						Total = 2457600
*********************************************************************************

*/



	delete sim;
}