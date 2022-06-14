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


/*!
 * \brief testing to make sure grpId error is caught in setNeuronMonitor.
 *
 */
TEST(setNrnMon, grpId) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim;
	const int GRP_SIZE = 10;

	// loop over both CPU and GPU mode.
	for (int mode = 0; mode < TESTED_MODES; mode++) {
		// first iteration, test CPU mode, second test GPU mode
		sim = new CARLsim("setNrnMon.grpId", mode ? GPU_MODE : CPU_MODE, SILENT, 1, 42);

		int g1 = sim->createGroup("g1", GRP_SIZE, EXCITATORY_NEURON);
		int g2 = sim->createGroup("g2", GRP_SIZE, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.0f, 0.2f, 0.0f, -65.0f, 0.0f, 8.0f, 0.0f);
		sim->setNeuronParameters(g2, 0.02f, 0.0f, 0.2f, 0.0f, -65.0f, 0.0f, 8.0f, 0.0f);

		EXPECT_DEATH(sim->setNeuronMonitor(ALL, "Default"), "");  // grpId = ALL (-1) and less than 0 
		EXPECT_DEATH(sim->setNeuronMonitor(-4, "Default"), "");  // less than 0
		EXPECT_DEATH(sim->setNeuronMonitor(2, "Default"), ""); // greater than number of groups

		delete sim;
	}
}


/*!
 * \brief testing to make sure file name error is caught in setNeuronMonitor.
 *
 */
TEST(setNrnMon, fname) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim;
	const int GRP_SIZE = 10;

	// loop over both CPU and GPU mode.
	for (int mode = 0; mode < TESTED_MODES; mode++) {
		// first iteration, test CPU mode, second test GPU mode
		sim = new CARLsim("setNrnMon.fname", mode ? GPU_MODE : CPU_MODE, SILENT, 1, 42);

		int g1 = sim->createGroup("g1", GRP_SIZE, EXCITATORY_NEURON);
		int g2 = sim->createGroup("g2", GRP_SIZE, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.0f, 0.2f, 0.0f, -65.0f, 0.0f, 8.0f, 0.0f);
		sim->setNeuronParameters(g2, 0.02f, 0.0f, 0.2f, 0.0f, -65.0f, 0.0f, 8.0f, 0.0f);

		// this directory doesn't exist.
		EXPECT_DEATH(sim->setNeuronMonitor(1, "absentDirectory/testNeurons.dat"), "");

		delete sim;
	}
}



TEST(NrnMon, interfaceDeath) {
	// use threadsafe version because we have deathtests
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	CARLsim* sim = new CARLsim("NrmMon.interfaceDeath", CPU_MODE, SILENT, 1, 42);

	int g1 = sim->createGroup("g1", 5, EXCITATORY_NEURON);
	sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);

	int g0 = sim->createSpikeGeneratorGroup("Input", 5, EXCITATORY_NEURON);
	NeuronMonitor* nrnMon = sim->setNeuronMonitor(g0, "NULL");

	sim->setConductances(true);

	sim->connect(g0, g1, "random", RangeWeight(0.01), 0.5f);

	// set up network and test all API calls that are not valid in certain modes
	sim->setupNetwork();

	// test all APIs that cannot be called when recording is on
	nrnMon->startRecording();

	EXPECT_DEATH(nrnMon->getVectorV(), "");
	EXPECT_DEATH(nrnMon->getVectorU(), "");
	EXPECT_DEATH(nrnMon->getVectorI(), "");
	EXPECT_DEATH(nrnMon->getLastUpdated(), "");
	EXPECT_DEATH(nrnMon->startRecording(), "");

	delete sim;
}



TEST(NrnMon, persistentMode) {

	CARLsim* sim = new CARLsim("NrnMon.persistentMode", CPU_MODE, SILENT, 1, 42);

	int g1 = sim->createGroup("g1", 5, EXCITATORY_NEURON);
	sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);

	int g0 = sim->createSpikeGeneratorGroup("Input", 5, EXCITATORY_NEURON);
	NeuronMonitor* nrnMon = sim->setNeuronMonitor(g0, "NULL");

	sim->setConductances(true);

	sim->connect(g0, g1, "random", RangeWeight(0.01), 0.5f);

	sim->setupNetwork();

	EXPECT_FALSE(nrnMon->getPersistentData());

	nrnMon->startRecording();
	sim->runNetwork(0, 10, true);
	nrnMon->stopRecording();

	auto lastUpdated = nrnMon->getLastUpdated();
	auto vectorV = nrnMon->getVectorV();
	EXPECT_EQ(lastUpdated, 10);
	EXPECT_EQ(vectorV[0].size(), 10);

	nrnMon->startRecording();
	sim->runNetwork(0, 10, true);
	nrnMon->stopRecording();

	lastUpdated = nrnMon->getLastUpdated();
	vectorV = nrnMon->getVectorV(); 
	EXPECT_EQ(lastUpdated, 20);
	EXPECT_EQ(vectorV[0].size(), 10); 


	// now switch persistent mode on, buffer is extended
	nrnMon->setPersistentData(true);

	EXPECT_TRUE(nrnMon->getPersistentData());

	nrnMon->startRecording();
	sim->runNetwork(0, 10, true);
	nrnMon->stopRecording();

	lastUpdated = nrnMon->getLastUpdated();
	vectorV = nrnMon->getVectorV();
	EXPECT_EQ(lastUpdated, 30);
	EXPECT_EQ(vectorV[0].size(), 20);

	nrnMon->startRecording();
	sim->runNetwork(0, 10, true);
	nrnMon->stopRecording();

	lastUpdated = nrnMon->getLastUpdated();
	vectorV = nrnMon->getVectorV();
	EXPECT_EQ(lastUpdated, 40);
	EXPECT_EQ(vectorV[0].size(), 30);

	delete sim;
}


TEST(NrnMon, VUI) {

	// loop over both CPU and GPU mode.
	for (int mode = 0; mode < TESTED_MODES; mode++) {

		// ---------------- CONFIG STATE -------------------
		CARLsim* sim = new CARLsim("NrnMon.VUI", CPU_MODE, SILENT, 1, 42);
		int n = 5;
		int g1 = sim->createGroup("g1", n, EXCITATORY_NEURON);
		sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);
		int g0 = sim->createSpikeGeneratorGroup("Input", 5, EXCITATORY_NEURON);
		sim->setConductances(false);
		sim->connect(g0, g1, "full", RangeWeight(100.f), 1.0f);
		NeuronMonitor* nrnMon = sim->setNeuronMonitor(g1, "NULL");
		nrnMon->setPersistentData(false); 

		// ---------------- SETUP STATE -------------------
		sim->setupNetwork();

		// ---------------- RUN STATE -------------------
		// 
		// e.g. wave prop iteration or CA3 activation
		std::vector<float> current(n, 0.f);	

		// init neural activity
		nrnMon->startRecording();
		current[0] = 100.f;
		sim->setExternalCurrent(g1, current);
		sim->runNetwork(0, 1, true);
		nrnMon->stopRecording();
		nrnMon->print();
		auto v = nrnMon->getVectorV();
		auto u = nrnMon->getVectorU();
		auto i = nrnMon->getVectorI();
		auto lastUpdated = nrnMon->getLastUpdated();

		EXPECT_EQ(v.size(), n+1); // stats
		EXPECT_EQ(u.size(), n+1);
		EXPECT_EQ(i.size(), n+1);
		EXPECT_EQ(lastUpdated, 1);  // simtime ms
		
		EXPECT_FLOAT_EQ(v[0][0], -65.0f); // initial value -65
		EXPECT_FLOAT_EQ(v[1][0], -65.0f);
		EXPECT_FLOAT_EQ(v[n-1][0], -65.0f);
		EXPECT_FLOAT_EQ(v[n][0], -65.0f); // basic stats 

		EXPECT_FLOAT_EQ(i[0][0], 100.0f); // initial value -65
		EXPECT_FLOAT_EQ(i[1][0], 0.0f);
		EXPECT_FLOAT_EQ(i[n-1][0], 0.0f);

		// spike
		current[0] = 0.f;
		sim->setExternalCurrent(g1, current);
		nrnMon->startRecording();
		sim->runNetwork(0, 1, true);
		nrnMon->stopRecording();
		nrnMon->print();
		v = nrnMon->getVectorV();
		u = nrnMon->getVectorU();
		i = nrnMon->getVectorI();
		lastUpdated = nrnMon->getLastUpdated();

		EXPECT_FLOAT_EQ(v[0][0], -65.0f); // initial value -65
		EXPECT_GT(v[0][0], v[1][0]); // next step
		EXPECT_FLOAT_EQ(v[1][0], v[n - 1][0]);
		EXPECT_LT(v[n][0], -65.0f); // mean is lower as some neurons have spiked 

		// hyper depolarization
		nrnMon->startRecording();
		sim->runNetwork(0, 1, true);
		nrnMon->stopRecording();
		nrnMon->print();
		v = nrnMon->getVectorV();
		u = nrnMon->getVectorU();
		i = nrnMon->getVectorI();
		lastUpdated = nrnMon->getLastUpdated();

		EXPECT_LT(v[0][0], v[1][0]); // next step
		EXPECT_FLOAT_EQ(v[1][0], v[n-1][0]);

		// basic stats, calc expected mean
		float v_sum = .0f;
		for (int i = 0; i < 5; i++) {
			v_sum += v[i][0]; 
		}
		EXPECT_FLOAT_EQ(v[n][0], v_sum / 5.0f); // mean(U) of neurons in the group 

		delete sim;
	}

}