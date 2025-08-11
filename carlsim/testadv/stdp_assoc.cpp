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

//#include <periodic_spikegen.h>
//#include <interactive_spikegen.h>
//#include <pre_post_group_spikegen.h>



/// **************************************************************************************************************** ///
/// SPIKE-TIMING-DEPENDENT PLASTICITY STDP
/// **************************************************************************************************************** ///


/*!
 * \brief testing the basic assication with symmetric E-STDP curve
 * This function tests whether the symmetric E-STDP change synaptic weight as expected
 * With control of pre- and post-neurons' spikes, the synaptic weight is expected to increase or decrease to
 * maximum or minimum synaptic weight respectively.
 */
TEST(STDP, ESTDPassoc_Adv) {
	
	// simulation details
	int size;
	int g_tof, g_vel, g_mot;

	// working medium f, calculated tau
	float I_MOT = 61.f;
	float I_TOF = 37.f;
	float ALPHA_LTP = 0.03f;
	float ALPHA_LTD = 0.03f;
	float TAU_LTP = 5.0f;
	float TAU_LTD = 5.0f;

	//// high f, narrow tau
	//float I_MOT = 80.f;
	//float I_TOF = 70.f;
	//float ALPHA_LTP = 0.02f;
	//float ALPHA_LTD = 0.02f;
	//float TAU_LTP = 3.0f;
	//float TAU_LTD = 3.0f;

	//// phase locking on low f
	//float I_MOT = 30.f;
	//float I_TOF = 20.f;
	//float ALPHA_LTP = 0.04f;
	//float ALPHA_LTD = -0.01f;
	//float TAU_LTP = 10.0f;
	//float TAU_LTD = 20.0f;

	//// default values do not associate 
	//float I_MOT = 30.f;
	//float I_TOF = 20.f;   // important, must be differnt I -> offset
	//float ALPHA_LTP = 0.10f;
	//float ALPHA_LTD = -0.14f;
	//float TAU_LTP = 20.0f;
	//float TAU_LTD = 20.0f;

	float maxWeight = 40.0f;
	float initWeight =  0.0f;
	float minWeight =  0.0f;

	//for (int mode = 0; mode < TESTED_MODES; mode++) {
		//for (int coba = 0; coba < 2; coba++) {
			//for (int offset = -30; offset <= 30; offset += 5) {
				//if (offset == 0) continue; // skip offset == 0;
				// create a network
				CARLsim* sim = new CARLsim("STDP.ESTDPExpCurve", CPU_MODE, SHOWTIME, 1, 42); // DEVELOPER

				g_tof = sim->createGroup("tof", 1, EXCITATORY_NEURON, 0);
				g_vel = sim->createGroup("vel", 1, EXCITATORY_NEURON, 0);
				g_mot = sim->createGroup("mot", 1, EXCITATORY_NEURON, 0);

				sim->setNeuronParameters(g_tof, 0.02f, 0.2f, -65.0f, 8.0f);
				sim->setNeuronParameters(g_vel, 0.02f, 0.2f, -65.0f, 8.0f);
				sim->setNeuronParameters(g_mot, 0.02f, 0.2f, -65.0f, 8.0f);

				// set current over time
				//gex1 = sim->createSpikeGeneratorGroup("input-ex1", 1, EXCITATORY_NEURON, 0);
				//gex2 = sim->createSpikeGeneratorGroup("input-ex2", 1, EXCITATORY_NEURON, 0);


				// training pathway
				sim->connect(g_mot, g_vel, "one-to-one", RangeWeight(40.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

				// associative, learning
				sim->connect(g_tof, g_vel, "one-to-one", RangeWeight(minWeight, initWeight, maxWeight), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);




				// set up ESTDP
				sim->setConductances(false);
				sim->setESTDP(g_tof, g_vel, true, STANDARD, ExpCurve(ALPHA_LTP, TAU_LTP, ALPHA_LTD, TAU_LTP));
				
				//// set up spike controller on DA neurons
				//sim->setSpikeGenerator(gex1, prePostSpikeGen);
				//sim->setSpikeGenerator(gex2, prePostSpikeGen);

				// build the network
				sim->setupNetwork();

				// training signal
				sim->setExternalCurrent(g_mot, I_MOT);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel

				// sensor feed
				sim->setExternalCurrent(g_tof, I_TOF);  // full, slow, non



				ConnectionMonitor* CM = sim->setConnectionMonitor(g_tof, g_vel, "NULL");
				CM->setUpdateTimeIntervalSec(-1);
				SpikeMonitor* SMtof = sim->setSpikeMonitor(g_tof, "Default");
				SpikeMonitor* SMvel = sim->setSpikeMonitor(g_vel, "Default");
				SpikeMonitor* SMmot = sim->setSpikeMonitor(g_mot, "Default");

				//SMtof->startRecording();
				//SMvel->startRecording();
				//SMmot->startRecording();

				for (int j = 0; j < 11; j++) {
					sim->runNetwork(5, 0, true);

					//SMtof->stopRecording();
					//SMvel->stopRecording();
					//SMmot->stopRecording();

					//SMtof->print(true);
					//SMvel->print(true);
					//SMmot->print(true);

					std::vector<std::vector<float> > weights = CM->takeSnapshot();

					printf("w:%f\n", weights[0][0]);


					//EXPECT_NEAR(maxInhWeight, weights[0][0], 0.5f);
				}



			// training signal
			// sensor feed
			sim->setExternalCurrent(g_mot, 0);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel
			sim->setExternalCurrent(g_tof, I_TOF);  // full, slow, non

			sim->runNetwork(1, 0, true);
			// Expected: new the same 
			//

			delete sim;
	
}


/*!
 * \brief testing the timing-based E-STDP curve
 * This function tests whether E-STDP change synaptic weight as expected
 * Wtih control of pre- and post-neurons' spikes, the synaptic weight is expected to increase or decrease to
 * maximum or minimum synaptic weith respectively.
 */

TEST(STDP, ESTDP_PulseCurve_assoc) {
	// simulation details

	int size;
	int g_tof, g_vel, g_mot;

	float I_MOT = 61.f;
	float I_TOF = 37.f;
	float BETA_LTP = 0.065f;   // associate
	float BETA_LTD = -0.04f;  // dis-assoc.
	float LAMBDA = 6.0f;    // d f 
	float DELTA = 18.0f;   // 

	float maxWeight = 40.0f;
	float initWeight = 0.0f;
	float minWeight = 0.0f;

	//for (int mode = 0; mode < TESTED_MODES; mode++) {
			//for (int coba = 0; coba < 2; coba++) {
				//for (int offset = -30; offset <= 30; offset += 5) {
					//if (offset == 0) continue; // skip offset == 0;
					// create a network

	CARLsim* sim = new CARLsim("STDP_PulseCurve", CPU_MODE, SHOWTIME, 1, 42);  //  DEVELOPER

	g_tof = sim->createGroup("tof", 1, EXCITATORY_NEURON, 0);
	g_vel = sim->createGroup("vel", 1, EXCITATORY_NEURON, 0);
	g_mot = sim->createGroup("mot", 1, EXCITATORY_NEURON, 0);

	sim->setNeuronParameters(g_tof, 0.02f, 0.2f, -65.0f, 8.0f);
	sim->setNeuronParameters(g_vel, 0.02f, 0.2f, -65.0f, 8.0f);
	sim->setNeuronParameters(g_mot, 0.02f, 0.2f, -65.0f, 8.0f);

	// set current over time
	//gex1 = sim->createSpikeGeneratorGroup("input-ex1", 1, EXCITATORY_NEURON, 0);
	//gex2 = sim->createSpikeGeneratorGroup("input-ex2", 1, EXCITATORY_NEURON, 0);


	// training pathway
	sim->connect(g_mot, g_vel, "one-to-one", RangeWeight(40.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

	// associative, learning

	sim->connect(g_tof, g_vel, "one-to-one", RangeWeight(minWeight, initWeight, maxWeight), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

	// set up ESTDP
	sim->setConductances(false);
	sim->setESTDP(g_tof, g_vel, true, STANDARD, PulseCurve(BETA_LTP, BETA_LTD, LAMBDA, DELTA));

	//// set up spike controller on DA neurons
	//sim->setSpikeGenerator(gex1, prePostSpikeGen);
	//sim->setSpikeGenerator(gex2, prePostSpikeGen);

	// build the network
	sim->setupNetwork();

	// training signal
	sim->setExternalCurrent(g_mot, I_MOT);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel

	// sensor feed
	sim->setExternalCurrent(g_tof, I_TOF);  // full, slow, non



	ConnectionMonitor* CM = sim->setConnectionMonitor(g_tof, g_vel, "NULL");
	CM->setUpdateTimeIntervalSec(-1);
	SpikeMonitor* SMtof = sim->setSpikeMonitor(g_tof, "Default");
	SpikeMonitor* SMvel = sim->setSpikeMonitor(g_vel, "Default");
	SpikeMonitor* SMmot = sim->setSpikeMonitor(g_mot, "Default");

	//SMtof->startRecording();
	//SMvel->startRecording();
	//SMmot->startRecording();

	printf("\n================ 1. learning association ===============\n");
	for (int j = 0; j < 5; j++) {
		sim->runNetwork(10, 0, true);

		//SMtof->stopRecording();
		//SMvel->stopRecording();
		//SMmot->stopRecording();

		//SMtof->print(true);
		//SMvel->print(true);
		//SMmot->print(true);

		std::vector<std::vector<float> > weights = CM->takeSnapshot();

		printf("w:%f\n", weights[0][0]);


		//EXPECT_NEAR(maxInhWeight, weights[0][0], 0.5f);
	}

	printf("\n================ 2. test assocation ===============\n\n");

	// training signal
	// sensor feed
	sim->setExternalCurrent(g_mot, 0);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel
	sim->setExternalCurrent(g_tof, I_TOF);  // full, slow, non

	sim->runNetwork(1, 0, true);
	// Expected: new the same 
	//

	// unlearning 

	// training signal
	sim->setExternalCurrent(g_mot, I_MOT * 0.25);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel

	// sensor feed
	sim->setExternalCurrent(g_tof, I_TOF * 0.25);  // full, slow, non

	printf("\n============== 3. unlearn assocation ===============\n");

	for (int j = 0; j <5 ; j++) {
		sim->runNetwork(10, 0, true);
		std::vector<std::vector<float> > weights = CM->takeSnapshot();

		printf("w:%f\n", weights[0][0]);
	}


	delete sim;

}






TEST(STDP, ESTDP_TimingBasedCurve_assoc) {
	// simulation details

	int size;
	int g_tof, g_vel, g_mot;

	float I_MOT = 61.f;
	float I_TOF = 37.f;
	float ALPHA_LTP = 0.08f;
	float ALPHA_LTD = -0.01f;  // -0.14f;
	float TAU_LTP = 20.0f;
	float TAU_LTD = 1.0f;
	//float GAMMA = 15.0f;
	float GAMMA = 16.0f;

	float maxWeight = 40.0f;
	float initWeight = 0.0f;
	float minWeight = 0.0f;

	//for (int mode = 0; mode < TESTED_MODES; mode++) {
			//for (int coba = 0; coba < 2; coba++) {
				//for (int offset = -30; offset <= 30; offset += 5) {
					//if (offset == 0) continue; // skip offset == 0;
					// create a network

	CARLsim* sim = new CARLsim("STDP_PulseCurve", CPU_MODE, SHOWTIME, 1, 42); //DEVELOPER

	g_tof = sim->createGroup("tof", 1, EXCITATORY_NEURON, 0);
	g_vel = sim->createGroup("vel", 1, EXCITATORY_NEURON, 0);
	g_mot = sim->createGroup("mot", 1, EXCITATORY_NEURON, 0);

	sim->setNeuronParameters(g_tof, 0.02f, 0.2f, -65.0f, 8.0f);
	sim->setNeuronParameters(g_vel, 0.02f, 0.2f, -65.0f, 8.0f);
	sim->setNeuronParameters(g_mot, 0.02f, 0.2f, -65.0f, 8.0f);

	// set current over time
	//gex1 = sim->createSpikeGeneratorGroup("input-ex1", 1, EXCITATORY_NEURON, 0);
	//gex2 = sim->createSpikeGeneratorGroup("input-ex2", 1, EXCITATORY_NEURON, 0);


	// training pathway
	sim->connect(g_mot, g_vel, "one-to-one", RangeWeight(40.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

	// associative, learning

	sim->connect(g_tof, g_vel, "one-to-one", RangeWeight(minWeight, initWeight, maxWeight), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

	// set up ESTDP
	sim->setConductances(false);
	sim->setESTDP(g_tof, g_vel, true, STANDARD, TimingBasedCurve(ALPHA_LTP, TAU_LTP, ALPHA_LTD, TAU_LTP, GAMMA));

	//// set up spike controller on DA neurons
	//sim->setSpikeGenerator(gex1, prePostSpikeGen);
	//sim->setSpikeGenerator(gex2, prePostSpikeGen);

	// build the network
	sim->setupNetwork();

	// training signal
	sim->setExternalCurrent(g_mot, I_MOT);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel

	// sensor feed
	sim->setExternalCurrent(g_tof, I_TOF);  // full, slow, non



	ConnectionMonitor* CM = sim->setConnectionMonitor(g_tof, g_vel, "NULL");
	CM->setUpdateTimeIntervalSec(-1);
	SpikeMonitor* SMtof = sim->setSpikeMonitor(g_tof, "Default");
	SpikeMonitor* SMvel = sim->setSpikeMonitor(g_vel, "Default");
	SpikeMonitor* SMmot = sim->setSpikeMonitor(g_mot, "Default");

	//SMtof->startRecording();
	//SMvel->startRecording();
	//SMmot->startRecording();

	printf("\n================ 1. learning association ===============\n");
	for (int j = 0; j < 11; j++) {
		sim->runNetwork(5, 0, true);

		//SMtof->stopRecording();
		//SMvel->stopRecording();
		//SMmot->stopRecording();

		//SMtof->print(true);
		//SMvel->print(true);
		//SMmot->print(true);

		std::vector<std::vector<float> > weights = CM->takeSnapshot();

		printf("w:%f\n", weights[0][0]);


		//EXPECT_NEAR(maxInhWeight, weights[0][0], 0.5f);
	}

	printf("\n================ 2. test assocation ===============\n\n");

	// training signal
	// sensor feed
	sim->setExternalCurrent(g_mot, 0);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel
	sim->setExternalCurrent(g_tof, I_TOF);  // full, slow, non

	sim->runNetwork(1, 0, true);
	// Expected: new the same 
	//



	// unlearning 

	// training signal
	sim->setExternalCurrent(g_mot, I_MOT * 0.25);  // expect -> f rate in g_mot, with with -> 1:1 freq in vel

	// sensor feed
	sim->setExternalCurrent(g_tof, I_TOF * 0.25);  // full, slow, non

	printf("\n============== 3. unlearn assocation ===============\n");

	for (int j = 0; j < 5; j++) {
		sim->runNetwork(10, 0, true);
		std::vector<std::vector<float> > weights = CM->takeSnapshot();

		printf("w:%f\n", weights[0][0]);
	}


	delete sim;

}
