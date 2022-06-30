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

#include <carlsim.h>

#include <stdio.h>
#include <vector>

#include <periodic_spikegen.h>
#include <interactive_spikegen.h>
#include <pre_post_group_spikegen.h>
 
int main(int argc, char* argv[]) {

	// provide simple command line interface with defaults  
	unsigned duration = 60;		auto arg1 = "1. simulation time in s (default = 60s)";
	unsigned coba = 0;			auto arg2 = "2. synapse: 1=COBA, 0=CUBA (default)";
	unsigned damod = 1;			auto arg3 = "3. modulation: 1=DA is released by activating gDA (default), 0= no DA release";
	unsigned type = 1;			auto arg4 = "4. stdp type: 1=DA_MOD (default), 0=STANDARD";
	unsigned mode = 1;			auto arg5 = "5. backend 1=GPU (default), 0=CPU";
	unsigned log = 0;			auto arg6 = "6. log 2=SILENT, 1=DEV, 0=USER (default)";

	if (argc > 1)
		duration = atoi(argv[1]);
	if (argc > 2)
		coba = atoi(argv[2]);
	if (argc > 3)
		damod = atoi(argv[3]);
	if (argc > 4)
		type = atoi(argv[4]);
	if (argc > 5)
		mode = atoi(argv[5]);
	if (argc > 6)
		log = atoi(argv[6]);

	if ((argc == 2 && strncmp("-h", argv[1], 2) == 0) || argc > 7 || mode > 1 || coba > 1 || damod > 1 || type > 1 || log > 2) {
		printf("usage: [duration [synapse [modulation [stdp [backend [log]]]]]]\n");
		printf("Arguments:\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n", arg1, arg2, arg3, arg4, arg5, arg6);
		return 0;
	}

	// ---------------- CONFIG STATE -------------------
	float tauPlus = 20.0f;
	float tauMinus = 20.0f;
	float alphaPlus = 0.1f;
	float alphaMinus = -0.1f;
	int g1, gin, g1noise, gda;
	InteractiveSpikeGenerator* iSpikeGen = new InteractiveSpikeGenerator(500, 500);
	std::vector<int> spikesPost;
	std::vector<int> spikesPre;
	SpikeMonitor* spikeMonPost;
	SpikeMonitor* spikeMonPre;

	std::vector<float> groupDataPost;
	GroupMonitor* groupMonPost; 

	CARLsim* sim = new CARLsim("DASTDP", mode ? GPU_MODE : CPU_MODE, LoggerMode(log), 1, 42);

	g1 = sim->createGroup("post-ex", 1, EXCITATORY_NEURON);
	sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);

	gin = sim->createSpikeGeneratorGroup("pre-ex", 1, EXCITATORY_NEURON);
	g1noise = sim->createSpikeGeneratorGroup("post-ex-noise", 1, EXCITATORY_NEURON);
	gda = sim->createSpikeGeneratorGroup("DA neurons", 500, DOPAMINERGIC_NEURON); // (2)

	STDPType stdp = type ? DA_MOD : STANDARD;   
												
	if (coba) {
		sim->connect(gin, g1, "one-to-one", RangeWeight(0.0, 1.0f / 100, 20.0f / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);
		sim->connect(g1noise, g1, "one-to-one", RangeWeight(40.0f / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(gda, g1, "full", RangeWeight(0.0), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		// enable COBA, set up STDP, enable dopamine-modulated STDP
		sim->setConductances(true, 5, 150, 6, 150);
		sim->setSTDP(gin, g1, true, stdp, alphaPlus / 100, tauPlus, alphaMinus / 100, tauMinus);
	}
	else { // cuba mode
		sim->connect(gin, g1, "one-to-one", RangeWeight(0.0, 1.0f, 20.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);
		sim->connect(g1noise, g1, "one-to-one", RangeWeight(40.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
		sim->connect(gda, g1, "full", RangeWeight(0.0), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);  // (1)
		// set up STDP, enable dopamine-modulated STDP
		sim->setSTDP(gin, g1, true, stdp, alphaPlus, tauPlus, alphaMinus, tauMinus);
		sim->setConductances(false);
	}
	
	sim->setNeuromodulator(g1, 2000.0f);
	sim->setWeightAndWeightChangeUpdate(INTERVAL_100MS, true, 0.95f);

	// set up spike controller on DA neurons
	sim->setSpikeGenerator(gda, iSpikeGen);

	sim->setupNetwork();

	ConnectionMonitor* CM = sim->setConnectionMonitor(gin, g1, "DEFAULT");

	spikeMonPost = sim->setSpikeMonitor(g1, "DEFAULT");
	spikeMonPre = sim->setSpikeMonitor(gin, "DEFAULT");
	sim->setSpikeMonitor(gda, "DEFAULT");

	groupMonPost = sim->setGroupMonitor(g1, "DEFAULT");

	//setup baseline firing rate
	PoissonRate in(1);
	in.setRates(4.0f); // 4Hz
	sim->setSpikeRate(gin, &in);
	sim->setSpikeRate(g1noise, &in);

	float weight =.0f;
	int releases = 0;

	for (unsigned t = 0; t < duration; t++) {
		spikeMonPost->startRecording();
		spikeMonPre->startRecording();
		groupMonPost->startRecording();
		sim->runNetwork(1, 0, false);
		spikeMonPost->stopRecording();
		spikeMonPre->stopRecording();
		groupMonPost->stopRecording();

		// get spike time of pre-synaptic neuron post-synaptic neuron
		spikesPre = spikeMonPre->getSpikeVector2D()[0]; // pre-neuron spikes
		spikesPost = spikeMonPost->getSpikeVector2D()[0]; // post-neuron in spikes

		////print DA concentration
		//auto groupValuesPost = groupMonPost->getDataVector();		
		//auto groupTimesPost = groupMonPost->getTimeVector();		
		//for(int i=0; i<groupValuesPost.size(); i++) {
		//	auto da_v = groupValuesPost[i];
		//	auto da_t = groupTimesPost[i];
		//	if(da_v > 1.0f)
		//		printf("%3ds: da[%d]=%f\n", t, da_t, da_v);
		//}

		// detect LTP or LTD
		// if LTP is detected, set up reward (activate DA neurons ) to reinforcement this synapse
		for (int j = 0; j < spikesPre.size(); j++) { // j: index of the (j+1)-th spike
			for (int k = 0; k < spikesPost.size(); k++) { // k: index of the (k+1)-th spike
				int diff = spikesPost[k] - spikesPre[j]; // (post-spike time) - (pre-spike time)
				if (diff > 0 && diff <= 20) {
					//printf("LTP\n");
					if (damod) {
						iSpikeGen->setQuotaAll(1);
						//printf("%3ds: release DA\n", t);
						releases++;
					}
				}
				//if (diff < 0 && diff >= -20)
				//	printf("LTD\n");
			}
		}

		std::vector< std::vector<float> > weights = CM->takeSnapshot();
		weight = weights[0][0];
		//printf("t: %ds w: %f\n", t, weight);  // interactive  verbose
	}

	delete sim;

	printf("time: %ds on %s synapse: %s da-stdp: %s da-releases: %d weight: %f\n", 
			duration, mode?"GPU":"CPU", coba?"COBA":"CUBA", damod?"y":"n", releases, weight);
}
