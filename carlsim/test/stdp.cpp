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
#include <interactive_spikegen.h>
#include <pre_post_group_spikegen.h>



/// **************************************************************************************************************** ///
/// SPIKE-TIMING-DEPENDENT PLASTICITY STDP
/// **************************************************************************************************************** ///

/*!
 * \brief testing setSTDP to true
 * This function tests the information stored in the group info struct after enabling STDP via setSTDP
 */
TEST(STDP, setSTDPTrue) {
	float alphaPlus = 5.0f;		// the exact values don't matter
	float alphaMinus = -10.0f;
	float tauPlus = 15.0f;
	float tauMinus = 20.0f;
	float gamma = 10.0f;
	float betaLTP = 1.0f;
	float betaLTD = -1.2f;
	float lambda = 12.0f;
	float delta = 40.0f;
	CARLsim* sim;

	for (int mode = 0; mode < TESTED_MODES; mode++) {
		for (int stdpType = 0; stdpType < 2; stdpType++) { // we have two stdp types {STANDARD, DA_MOD}
			for(int stdpCurve = 0; stdpCurve < 2; stdpCurve++) { // we have four stdp curves, two for ESTDP, two for ISTDP
				sim = new CARLsim("STDP.setSTDPTrue",mode?GPU_MODE:CPU_MODE,SILENT,1,42);

				int g1 = sim->createGroup("excit", 10, EXCITATORY_NEURON);
				int g2 = sim->createGroup("excit", 10, EXCITATORY_NEURON);
				int g3 = sim->createGroup("inhib", 10, INHIBITORY_NEURON);

				short int connId = sim->connect(g1, g2, "one-to-one", RangeWeight(0.0, 1.0f / 100, 20.0f / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);
				short int connId2 = sim->connect(g3, g2, "one-to-one", RangeWeight(0.0, 1.0f / 100, 20.0f / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

				sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);
				if (stdpType == 0) {
					if (stdpCurve == 0) {
						sim->setESTDP(g1, g2, true, STANDARD, ExpCurve(alphaPlus,tauPlus,alphaMinus,tauMinus));
						sim->setISTDP(g3, g2, true, STANDARD, ExpCurve(alphaPlus,tauPlus,alphaMinus,tauMinus));
					} else { //stdpCurve == 1
						sim->setESTDP(g1, g2, true, STANDARD, TimingBasedCurve(alphaPlus,tauPlus,alphaMinus,tauMinus, gamma));
						sim->setISTDP(g3, g2, true, STANDARD, PulseCurve(betaLTP,betaLTD,lambda,delta));
					}
				} else { // stdpType == 1
					if (stdpCurve == 0) {
						sim->setESTDP(g1, g2, true, DA_MOD, ExpCurve(alphaPlus,tauPlus,alphaMinus,tauMinus));
						sim->setISTDP(g3, g2, true, DA_MOD, ExpCurve(alphaPlus,tauPlus,alphaMinus,tauMinus));
					} else { //stdpCurve == 1
						sim->setESTDP(g1, g2, true, DA_MOD, TimingBasedCurve(alphaPlus,tauPlus,alphaMinus,tauMinus, gamma));
						sim->setISTDP(g3, g2, true, DA_MOD, PulseCurve(betaLTP,betaLTD,lambda,delta));
					}
				}

				ConnSTDPInfo gInfo = sim->getConnSTDPInfo(connId);
				ConnSTDPInfo gInfo2 = sim->getConnSTDPInfo(connId2);
				EXPECT_TRUE(gInfo.WithSTDP);
				EXPECT_TRUE(gInfo.WithESTDP);
				EXPECT_TRUE(gInfo2.WithISTDP);
				if (stdpType == 0) {
					EXPECT_TRUE(gInfo.WithESTDPtype == STANDARD);
					EXPECT_TRUE(gInfo2.WithISTDPtype == STANDARD);
				} else { // stdpType == 1
					EXPECT_TRUE(gInfo.WithESTDPtype == DA_MOD);
					EXPECT_TRUE(gInfo2.WithISTDPtype == DA_MOD);
				}

				if (stdpCurve == 0) {
					EXPECT_TRUE(gInfo.WithESTDPcurve == EXP_CURVE);
					EXPECT_TRUE(gInfo2.WithISTDPcurve == EXP_CURVE);
				} else {
					EXPECT_TRUE(gInfo.WithESTDPcurve == TIMING_BASED_CURVE);
					EXPECT_TRUE(gInfo2.WithISTDPcurve == PULSE_CURVE);
				}

				EXPECT_FLOAT_EQ(gInfo.ALPHA_PLUS_EXC,alphaPlus);
				EXPECT_FLOAT_EQ(gInfo.ALPHA_MINUS_EXC,alphaMinus);
				EXPECT_FLOAT_EQ(gInfo.TAU_PLUS_INV_EXC,1.0/tauPlus);
				EXPECT_FLOAT_EQ(gInfo.TAU_MINUS_INV_EXC,1.0/tauMinus);
				if (stdpCurve == 0) {
					EXPECT_FLOAT_EQ(gInfo2.ALPHA_PLUS_INB,alphaPlus);
					EXPECT_FLOAT_EQ(gInfo2.ALPHA_MINUS_INB,alphaMinus);
					EXPECT_FLOAT_EQ(gInfo2.TAU_PLUS_INV_INB,1.0/tauPlus);
					EXPECT_FLOAT_EQ(gInfo2.TAU_MINUS_INV_INB,1.0/tauMinus);
					EXPECT_FLOAT_EQ(gInfo.GAMMA, 0.0f);
				} else {
					EXPECT_FLOAT_EQ(gInfo2.BETA_LTP,betaLTP);
					EXPECT_FLOAT_EQ(gInfo2.BETA_LTD,betaLTD);
					EXPECT_FLOAT_EQ(gInfo2.LAMBDA,lambda);
					EXPECT_FLOAT_EQ(gInfo2.DELTA,delta);
					EXPECT_FLOAT_EQ(gInfo.GAMMA, gamma);
				}

				delete sim;
			}
		}
	}
}

/*!
 * \brief testing setSTDP to false
 * This function tests the information stored in the group info struct after disabling STDP via setSTDP
 */
TEST(STDP, setSTDPFalse) {
	float alphaPlus = 5.0f;		// the exact values don't matter
	float alphaMinus = -10.0f;
	float tauPlus = 15.0f;
	float tauMinus = 20.0f;
	float betaLTP = 1.0f;
	float betaLTD = -2.0f;
	float lambda = 3.0f;
	float delta = 4.0f;
	CARLsim* sim;

	for (int mode = 0; mode < TESTED_MODES; mode++) {
		sim = new CARLsim("STDP.setSTDPFalse",mode?GPU_MODE:CPU_MODE,SILENT,1,42);

		int g1=sim->createGroup("excit1", 10, EXCITATORY_NEURON);
		int g2 = sim->createGroup("excit2", 10, EXCITATORY_NEURON);
		int g3 = sim->createGroup("inhib", 10, INHIBITORY_NEURON);

		short int connId = sim->connect(g1, g2, "one-to-one", RangeWeight(0.0, 1.0f / 100, 20.0f / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);
		short int connId2 = sim->connect(g3, g2, "one-to-one", RangeWeight(0.0, 1.0f / 100, 20.0f / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

		sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);
		sim->setESTDP(g1,g2,false,STANDARD, ExpCurve(alphaPlus,tauPlus,alphaMinus,tauMinus));
		sim->setISTDP(g3,g2,false,STANDARD, PulseCurve(betaLTP,betaLTD,lambda,delta));

		ConnSTDPInfo gInfo = sim->getConnSTDPInfo(connId);
		ConnSTDPInfo gInfo2 = sim->getConnSTDPInfo(connId2);
		EXPECT_FALSE(gInfo.WithSTDP);
		EXPECT_FALSE(gInfo2.WithSTDP);
		EXPECT_FALSE(gInfo.WithESTDP);
		EXPECT_FALSE(gInfo.WithISTDP);
		EXPECT_FALSE(gInfo2.WithESTDP);
		EXPECT_FALSE(gInfo2.WithISTDP);

		EXPECT_FLOAT_EQ(gInfo.ALPHA_PLUS_EXC, 0.0f);
		EXPECT_FLOAT_EQ(gInfo.ALPHA_MINUS_EXC, 0.0f);
		EXPECT_FLOAT_EQ(gInfo.TAU_PLUS_INV_EXC, 1.0f);
		EXPECT_FLOAT_EQ(gInfo.TAU_MINUS_INV_EXC, 1.0f);
		EXPECT_FLOAT_EQ(gInfo.ALPHA_PLUS_EXC, 0.0f);
		EXPECT_FLOAT_EQ(gInfo.ALPHA_MINUS_EXC, 0.0f);
		EXPECT_FLOAT_EQ(gInfo.TAU_PLUS_INV_EXC, 1.0f);
		EXPECT_FLOAT_EQ(gInfo.TAU_MINUS_INV_EXC, 1.0f);
		EXPECT_FLOAT_EQ(gInfo2.BETA_LTP, 0.0f);
		EXPECT_FLOAT_EQ(gInfo2.BETA_LTD, 0.0f);
		EXPECT_FLOAT_EQ(gInfo2.LAMBDA, 1.0f);
		EXPECT_FLOAT_EQ(gInfo2.DELTA, 1.0f);
		EXPECT_FLOAT_EQ(gInfo.GAMMA, 0.0f);

		delete sim;
	}
}

/*!
 * \brief testing setSTDPNeuromodulatorParameters
 * This function tests the information stored in the group info struct after setting neuromodulator parameters
 */
TEST(STDP, setNeuromodulatorParameters) {
	float alphaPlus = 1.0f;		// the exact values don't matter
	float alphaMinus = 1.2f;
	float tauPlus = 20.0f;
	float tauMinus = 20.0f;
	float baseDP = 1.0f;
	float base5HT = 2.0f;
	float baseACh = 3.0f;
	float baseNE = 4.0f;
	float tauDP = 100.0f;
	float tau5HT = 200.0f;
	float tauACh = 300.0f;
	float tauNE = 400.0f;
	CARLsim* sim;

	for (int mode = 0; mode < TESTED_MODES; mode++) {
		sim = new CARLsim("STDP.setNeuromodulatorParameters",mode?GPU_MODE:CPU_MODE,SILENT,1,42);

		int g1=sim->createGroup("excit", 10, EXCITATORY_NEURON);
		int g2=sim->createGroup("excit", 10, EXCITATORY_NEURON);

		short int connId = sim->connect(g1,g2,"one-to-one", RangeWeight(0.0, 1.0f/100, 20.0f/100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

		sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);
		sim->setSTDP(g1,g2,true,DA_MOD,alphaPlus,tauPlus,alphaMinus,tauMinus);
		sim->setNeuromodulator(g1, baseDP, tauDP, base5HT, tau5HT, baseACh, tauACh, baseNE, tauNE);

		// Temporarily mark out the testing code
		// Discuss whether carlsim user interface needs to spport GroupConfigRT
		ConnSTDPInfo gInfo = sim->getConnSTDPInfo(g1);
		EXPECT_TRUE(gInfo.WithSTDP);
		EXPECT_TRUE(gInfo.WithESTDPtype == DA_MOD);

		GroupNeuromodulatorInfo gInfo2 = sim->getGroupNeuromodulatorInfo(g1);
		EXPECT_FLOAT_EQ(gInfo2.baseDP, baseDP);
		EXPECT_FLOAT_EQ(gInfo2.base5HT, base5HT);
		EXPECT_FLOAT_EQ(gInfo2.baseACh, baseACh);
		EXPECT_FLOAT_EQ(gInfo2.baseNE, baseNE);
		EXPECT_FLOAT_EQ(gInfo2.decayDP, 1.0f - 1.0f / tauDP);
		EXPECT_FLOAT_EQ(gInfo2.decay5HT, 1.0f - 1.0f / tau5HT);
		EXPECT_FLOAT_EQ(gInfo2.decayACh, 1.0f - 1.0f / tauACh);
		EXPECT_FLOAT_EQ(gInfo2.decayNE, 1.0f - 1.0f / tauNE);

		delete sim;
	}
}


/*!
* \brief testing Homeostasis
* Test Homeostasis for CPU and GPU
* --gtest_filter=STDP.homeostasis
*/
TEST(STDP, homeostasis) {

	// CARLsim parameter
	LoggerMode logger = SILENT;  // USER, SILENT
	int numGPUs = 0;
	int randSeed = 42;

	int nPois = 100; // 100 input neurons
	int nExc = 1;   // 1 output neuron

	// run the established network 
	int runTimeSec = 25; // seconds

	for (int mode = 0; mode < TESTED_MODES; mode++) {

		CARLsim sim("homeostasis_test", mode ? GPU_MODE : CPU_MODE, logger, 1, randSeed);

		int gPois = sim.createSpikeGeneratorGroup("input", nPois, EXCITATORY_POISSON);
		int gExc = sim.createGroup("output", nExc, EXCITATORY_NEURON);
		sim.setNeuronParameters(gExc, 0.02f, 0.2f, -65.0f, 8.0f);

		// connect our groups with SYN_PLASTIC as the final argument.
		sim.connect(gPois, gExc, "full", RangeWeight(0.0, 0.01, 0.03), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

		// set conductances with default values
		sim.setConductances(true);

		// create PoissonRate object of size nPoiss.
		PoissonRate poissRate(nPois);

		// set E-STDP parameters.
		float alpha_LTP = 0.001f / 5; float tau_LTP = 20.0f;
		float alpha_LTD = 0.00033f / 5; float tau_LTD = 60.0f;

		// set E-STDP to be STANDARD (without neuromodulatory influence) with an EXP_CURVE type.
		sim.setESTDP(gPois, gExc, true, STANDARD, ExpCurve(alpha_LTP, tau_LTP, -alpha_LTD, tau_LTD));

		// homeostasis constants
		float alpha = 1.0f; // homeostatic scaling factor
		float T = 5.0f; // homeostatic time constant
		float R_target = 35.0f; // target firing rate neuron tries to achieve

		sim.setHomeostasis(gExc, true, alpha, T);
		sim.setHomeoBaseFiringRate(gExc, R_target);

		// ---------------- SETUP STATE -------------------
		sim.setupNetwork();
		SpikeMonitor* SpikeMonOutput = sim.setSpikeMonitor(gExc, "DEFAULT");

		// ---------------- RUN STATE -------------------
		// set rate of each neuron
		std::vector <float> rates;
		for (int i = 0; i < nPois; i++)
			rates.push_back((i + 1) * (20.0f / 100));

		poissRate.setRates(rates);
		sim.setSpikeRate(gPois, &poissRate);

		float sOut, R_min = 1000.f, R_max = 0.f;
		for (int t = 0; t < runTimeSec; t++) {
			SpikeMonOutput->startRecording();
			sim.runNetwork(1, 0, true);
			SpikeMonOutput->stopRecording();
			sOut = SpikeMonOutput->getPopMeanFiringRate();
#if defined(WIN32) && defined(__NO_CUDA__) 
			// MS VC
			R_min = std::min<float>(R_min, sOut);
			R_max = std::max<float>(R_max, sOut);
#else
			// C++11 <algorithm>
#undef min
#undef max
			R_min = std::min(R_min, sOut);
			R_max = std::max(R_max, sOut);
#endif
		}
		EXPECT_NEAR (R_target, sOut, 1.5f); // Hz
		EXPECT_NEAR (R_min, 21.0f, 3.0f);
		EXPECT_NEAR (R_max, 56.0f, 3.0f);
	}
}