#include <carlsim.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <spikegen_from_vector.h>
#include <normal_spikegen2.h>
#include <snn_definitions.h>

#include <iostream>

int main(int argc, const char* argv[]) {

	CARLsim::InitParams(argc, argv, "carlsim.conf");


	bool bSpikeMon = false;
	bool bPerfMon = false;

	// ---------------- CONFIG STATE -------------------
#ifdef __NO_CUDA__
	CARLsim sim("synfire_network", CPU_MODE, USER, 5);
#else
	CARLsim sim("synfire_network", GPU_MODE, DEVELOPER);
#endif

	int partitions = CARLsim::Params()[CUSTOM_1_PARAM];

	int nGroups = 4;			// number of synfire chain groups
	int nNeur = 250;			// number of neurons
	int nNeurExc = 0.8*nNeur;	// number of excitatory neurons
	int nNeurInh = 0.2*nNeur;	// number of inhibitory neurons
	int nSynPerNeur = 60;  		// number of synpases per neuron
	int maxDelay = 20;      	// maximal conduction delay
	int nSynExc = 60; 			// number of neurons connected from previous group's Exciatory
	int nSynInh = 25;			// number of neurons from inhibitory to exciatory in same group

	std::vector<int> gExc(nGroups);
	std::vector<int> gInh(nGroups);

	//Different than normal exciatory
	int spiking_group = sim.createSpikeGeneratorGroup("spike",nNeurExc,EXCITATORY_NEURON,0);

	NormalSpikeGenerator2 spikegen =  NormalSpikeGenerator2(10.0f, 2.4f, 400, false);

	// Create Synfire Chain Groups
	for (int i = 0; i < nGroups; i++) {
		auto p = partitions > 0 ? (i % partitions) + 1 : 0;
		std::string gExcName = "exc" + std::to_string(i);
		std::string gInhName = "inh" + std::to_string(i);
		gExc[i] = sim.createGroup(gExcName.c_str(),nNeurExc,EXCITATORY_NEURON, p);
		sim.setNeuronParameters(gExc[i], 0.02f, 0.2f, -65.0f, 8.0f);

		gInh[i] = sim.createGroup(gInhName.c_str(),nNeurInh,INHIBITORY_NEURON,p);
		sim.setNeuronParameters(gInh[i], 0.1f, 0.2f, -65.0f, 2.0f);
	}
	

	// specify connectivity
	float wtExc = 0.019f;                   // synaptic weight magnitude if pre is exc
	float wtInh = 0.076f;                   // synaptic weight magnitude if pre is inh (no negative sign)
	//float wtMax = 10.0f;                  // maximum synaptic weight magnitude

	sim.setSpikeGenerator(spiking_group,&spikegen);

	//Connect the spiking generator to the first group
	sim.connect(spiking_group, gExc[0], "random", RangeWeight(wtExc), (float)nSynExc/nNeurExc, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);
	// gExc receives input from nSynPerNeur neurons from both gExc and gInh
	// every neuron in gExc should receive ~nSynPerNeur synapses
	for (int i = 0; i < nGroups; i++){
		sim.connect(gInh[i], gExc[i], "random", RangeWeight(wtInh), (float)nSynInh/nNeurExc, RangeDelay(8,8), RadiusRF(-1), SYN_FIXED);

		if(i < nGroups-1){
			sim.connect(gExc[i], gExc[i + 1], "random", RangeWeight(wtExc), (float)nSynExc/nNeurExc, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);
            sim.connect(gExc[i], gInh[i + 1], "random", RangeWeight(wtExc), (float)nSynExc/nNeurExc, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);		
		}
	}
	//Connecting the last group back to the first
	sim.connect(gExc[3], gExc[0], "random", RangeWeight(wtExc), (float)nSynExc/nNeurExc, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);
	sim.connect(gExc[3], gInh[0], "random", RangeWeight(wtExc), (float)nSynExc/nNeurExc, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);
	//Connect the third group back to the first
	//sim.connect(gExc[3], gExc[0], "random", RangeWeight(wtExc), (float)nSynExc/nNeurExc, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);
	//sim.connect(gExc[3], gInh[0], "random", RangeWeight(wtExc), (float)nSynExc/nNeurExc, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);	
	// gInh receives input from nSynPerNeur neurons from gExc, all delays are 1ms, no plasticity
	// every neuron in gInh should receive ~nSynPerNeur synapses

	// enable STDP on all incoming synapses to gExc
	// float alphaPlus = 0.1f, tauPlus = 20.0f, alphaMinus = 0.1f, tauMinus = 20.0f;
	// for (int i = 0; i < nGroups; i++) {
    //     sim.setSTP(gExc[i], true, 0.15f, 750.0f, 50.0f);
    //     sim.setSTP(gExc[i], true, 0.15f, 750.0f, 50.0f);
    // }

	// run COBA mode
	sim.setConductances(true);

	//PerformanceMonitor* perfMon = sim.setPerformanceMonitor(PMB_INTEL, "DEFAULT");

	// ---------------- SETUP STATE -------------------
	sim.setupNetwork();

	
	std::vector<SpikeMonitor*> SMexc(nGroups);
    //std::vector<SpikeMonitor*> SMinh(nGroups);

	std::vector<ConnectionMonitor*> CMee(nGroups);
	
	//perfMon->setSampleRate(10);
	if (bSpikeMon) {
		//std::vector<ConnectionMonitor*> CMei(nGroups);
		SMexc[0] = sim.setSpikeMonitor(gExc[3], "DEFAULT");
		for (int i = 0; i < nGroups; i++) {
			//SMexc[i] = sim.setSpikeMonitor(gExc[i], "DEFAULT");
			//SMinh[i] = sim.setSpikeMonitor(gInh[i], "DEFAULT");
			//CMei[i] = sim.setConnectionMonitor(gInh[i], gExc[i], "DEFAULT");
			if (i < nGroups - 1) {
				//CMee[i] = sim.setConnectionMonitor(gExc[i], gExc[i+1], "DEFAULT");
			}
		}
		//SpikeMonitor* SMinput = sim.setSpikeMonitor(spiking_group, "DEFAULT");
	//	SMinput->print(true);
	}

    // ---------------- RUN STATE -------------------
	//perfMon->startRecording();
	if (bSpikeMon) {
		//SMinput->startRecording();
		SMexc[0]->startRecording();
		for (int i = 0; i < nGroups; i++) {
			//SMexc[i]->startRecording();
			//SMinh[i]->startRecording();
			//SMexc[i]->print(false);
			//SMinh[i]->print(false);
		}
	}
	
	
	int T = CARLsim::Params()[CUSTOM_2_PARAM];
	if (T == -1)
		T = 3;  // sec

	//t in ms
    for (int t = 0; t < 5*10*T; t++) {
    //for (int t = 0; t < 10*T; t++) {
		//std::cout << "t: " << t << std::endl;
        //std::vector<float> thalamCurrExc(nNeurExc, 0.0f);
        // Stimulate the first group

		//Calculate the normal distribution value
		/*double s = 1.6;
		double m  = 10;
		const double pi = 3.141592653589793;
		double coeff = 200.0 / (s * std::sqrt(2 * pi));
		double exponent = -0.5 * std::pow((t - m) / s, 2);
		double norm = coeff * std::exp(exponent);
		//Set the noise in the current
		for (int j = 0; j < norm; j++) {
			thalamCurrExc[j] = 80.0f;
		}
        sim.setExternalCurrent(gExc[3], thalamCurrExc);*/
        // Run for 1 ms
        //sim.runNetwork(0, 20, true);
		sim.runNetwork(0, 20, false);
		//sim.runNetwork(0, 100, false);
	}

    // Stop Recording & Print Stats
	//perfMon->stopRecording();
	if (bSpikeMon) {
		//SMinput->stopRecording();
		SMexc[0]->stopRecording();
	}
    for (int i = 0; i < nGroups; i++) {
		if (bSpikeMon) {
			//SMexc[i]->stopRecording();
			//SMinh[i]->stopRecording();
			//SMexc[i]->print(false);
			//SMinh[i]->print(false);
		}
    }
	return 0;
}
