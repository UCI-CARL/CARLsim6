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
#include <vector>

#include <periodic_spikegen.h>
#include <spikegen_from_file.h>
#include <spikegen_from_vector.h>

#include <normal_spikegen.h>




// tests whether the binary spike file created by setSpikeMonitor contains the same spike times as specified
// by a spike vector
// timeslice 25
TEST(spikeGenFunc, NormalSpikeGenerator25) {


	int nNeur = 10;
	CARLsim sim("NormalSpikeGenerator", CPU_MODE, SILENT, 0, 42);

	int g1 = sim.createGroup("g1", 1, EXCITATORY_NEURON);
	sim.setNeuronParameters(g1, 0.02, 0.2, -65.0, 8.0);

	int g0 = sim.createSpikeGeneratorGroup("Input", nNeur, EXCITATORY_NEURON);
	//NormalSpikeGenerator spkGen(10, 2.8, 20);
	NormalSpikeGenerator spkGen(10, 2.8, 100);
	sim.setSpikeGenerator(g0, &spkGen);

	sim.setConductances(true);

	// add some dummy connections so we can actually run the network
	sim.connect(g0, g1, "random", RangeWeight(0.01), 0.5f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

	sim.setupNetwork();
	sim.setSpikeMonitor(g0, "spkInputGrp0.dat"); // save spikes to file
	sim.runNetwork(0, 25);

	//// explicitly read the spike file to make sure
	//int* inputArray0 = NULL;
	//long inputSize0;
	////readAndReturnSpikeFile("spkInputGrp0.dat", inputArray0, inputSize0);
	////bool isSize0Correct = inputSize0 / 2 == spkTimes.size();
	////EXPECT_TRUE(isSize0Correct);

	////if (isSize0Correct) {
	////	for (int i = 0; i < spkTimes.size(); i++) {
	////		EXPECT_EQ(inputArray0[i * 2], spkTimes[i]);
	////	}
	////}

	////if (inputArray0 != NULL) delete[] inputArray0;

	EXPECT_TRUE(true);
}



// tests whether the binary spike file created by setSpikeMonitor contains the same spike times as specified
// by a spike vector
// timeslice 25
TEST(spikeGenFunc, NormalSpikeGenerator1) {


	int nNeur = 10;  // cut-off events > 73
	//int nNeur = 20;  // cut-off events > 73
	CARLsim sim("NormalSpikeGenerator", CPU_MODE, SILENT, 0, 42);

	int g1 = sim.createGroup("g1", 1, EXCITATORY_NEURON);
	sim.setNeuronParameters(g1, 0.02, 0.2, -65.0, 8.0);

	int g0 = sim.createSpikeGeneratorGroup("Input", nNeur, EXCITATORY_NEURON);
	//NormalSpikeGenerator spkGen(10, 2.8, 20);
	//NormalSpikeGenerator spkGen(10, 2.8, 40);
	//NormalSpikeGenerator spkGen(10, 2.8, 50);
	NormalSpikeGenerator spkGen(10, 2.8, 60);
	//NormalSpikeGenerator spkGen(10, 2.8, 73);  // OK 
	//NormalSpikeGenerator spkGen(10, 2.8, 74);  // OK with 20,   NOK  10 CUT OFF see  CAUTION: int n = round(mean * 2 + 1);   < <=   Fix in routine !!!
	//NormalSpikeGenerator spkGen(10, 2.8, 80);
	//NormalSpikeGenerator spkGen(10, 2.8, 100);  // OK 20
	//NormalSpikeGenerator spkGen(10, 2.8, 200);  // NOK 20 110 / 198 -> ok, does not work yet
	sim.setSpikeGenerator(g0, &spkGen);

	sim.setConductances(true);

	// add some dummy connections so we can actually run the network
	sim.connect(g0, g1, "random", RangeWeight(0.01), 0.5f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

	sim.setupNetwork();
	sim.setSpikeMonitor(g0, "results/spk_g0.dat"); // save spikes to file
	/*
		GM = GroupMonitor('g0','results');
		GM.hasValidSpikeFile()
		GM.plot('raster',[1],25);
	*/

	for (int t = 0; t < 25; t++) {
		//printf("timeslice %d\n", t);
		sim.runNetwork(0, 1);  // timeslice !!!
	}

	//// read the spike file and print out contents
	//int* aerArray = NULL;  // { (t, nid) }*
	//long arraySize;
	//readAndReturnSpikeFile("results/spk_g0.dat", aerArray, arraySize);
	//for (int i = 0; i < arraySize; i += 2) {
	//	int t = aerArray[i];
	//	int nid = aerArray[i + 1];
	//	printf("%3d ms: %d\n", t, nid);
	//}
	//if (aerArray != NULL) delete[] aerArray;

	// explicitly read the spike file to make sure
	int* inputArray0 = NULL;
	long inputSize0;
	readAndReturnSpikeFile("results/spk_g0.dat", inputArray0, inputSize0);
	EXPECT_EQ(inputSize0 / 2, spkGen.samples());
	bool isSize0Correct = inputSize0 / 2 == spkGen.samples();
	EXPECT_TRUE(isSize0Correct);
	if (inputArray0 != NULL) delete[] inputArray0;




	EXPECT_TRUE(true);
}



// tests whether the binary spike file created by setSpikeMonitor contains the same spike times as specified
// by a spike vector
// timeslice 100
TEST(spikeGenFunc, NormalSpikeGeneratorPeriod100) {


	int nNeur = 20;
	CARLsim sim("NormalSpikeGenerator", CPU_MODE, SILENT, 0, 42);

	int g1 = sim.createGroup("g1", 1, EXCITATORY_NEURON);
	sim.setNeuronParameters(g1, 0.02, 0.2, -65.0, 8.0);

	int g0 = sim.createSpikeGeneratorGroup("Input", nNeur, EXCITATORY_NEURON);
	//NormalSpikeGenerator spkGen(10, 2.8, 20, 200, false);
	NormalSpikeGenerator spkGen(10, 2.8, 100, 200, false);
	sim.setSpikeGenerator(g0, &spkGen);

	sim.setConductances(true);

	// add some dummy connections so we can actually run the network
	sim.connect(g0, g1, "random", RangeWeight(0.01), 0.5f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

	sim.setupNetwork();
	sim.setSpikeMonitor(g0, "spkInputGrp0.dat"); // save spikes to file

	// run for 2s with 100ms timeslice 
// expected: currentTime increase in 100 steps
// so if period 200 would cause 5 runs of the SpikeGen
// resulting in 5*21 = 105 spikes
	for (int t = 0; t < 20; t++) {
		//printf("timeslice %d\n", t);
		sim.runNetwork(0, 100);
		// spikes?
	}


//	// run for 2s with 100ms timeslice 
//// expected: currentTime increase in 100 steps
//// so if period 200 would cause 5 runs of the SpikeGen
//// resulting in 5*21 = 105 spikes
//	for (int t = 0; t < 20; t++) {
//		//printf("timeslice %d\n", t);
//		for (int ms = 0; ms < 100; ms++) {
//			sim.runNetwork(0, 1);
//		}
//		// #spikes = 1000
//	}
//
//


	EXPECT_TRUE(true);
}





// tests whether the binary spike file created by setSpikeMonitor contains the same spike times as specified
// by a spike vector
// timeslice 100
TEST(spikeGenFunc, NormalSpikeGeneratorPeriod1) {


	int nNeur = 20;
	CARLsim sim("NormalSpikeGenerator", CPU_MODE, SILENT, 0, 42);

	int g1 = sim.createGroup("g1", 1, EXCITATORY_NEURON);
	sim.setNeuronParameters(g1, 0.02, 0.2, -65.0, 8.0);

	int g0 = sim.createSpikeGeneratorGroup("Input", nNeur, EXCITATORY_NEURON);
	NormalSpikeGenerator spkGen(10, 2.8, 20, 200, false);
	//NormalSpikeGenerator spkGen(10, 2.8, 100, 200, false);
	sim.setSpikeGenerator(g0, &spkGen);

	sim.setConductances(true);

	// add some dummy connections so we can actually run the network
	sim.connect(g0, g1, "random", RangeWeight(0.01), 0.5f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

	sim.setupNetwork();
	sim.setSpikeMonitor(g0, "spkInputGrp0.dat"); // save spikes to file

	// run for 2s with 100ms timeslice 
	// expected: currentTime increase in 100 steps
	// so if period 200 would cause 5 runs of the SpikeGen
	// resulting in 5*21 = 105 spikes
	//for (int t = 0; t < 20; t++) {
	for (int t = 0; t < 2000; t++) {
			//printf("timeslice %d\n", t);
		sim.runNetwork(0, 1);
	}



	EXPECT_TRUE(true);
}


//TEST(spikeGenFunc, NormalSpikeGeneratorDeath) {
//	::testing::FLAGS_gtest_death_test_style = "threadsafe";
//
//	//std::vector<int> emptyVec, negativeVec;
//	//negativeVec.push_back(0);
//	//negativeVec.push_back(-1);
//
//	EXPECT_DEATH({ NormalSpikeGenerator spkGen(0, -1, 0); }, "");
//
//}
