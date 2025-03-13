#include <carlsim.h>
#include <vector>
#include <cmath>
#include <cstdlib>


int main(int argc, const char* argv[]) {
	// ---------------- CONFIG STATE -------------------
#ifdef __NO_CUDA__
	CARLsim sim("synfire_network", CPU_MODE, USER);
#else
	CARLsim sim("synfire_network", GPU_MODE, USER);
#endif
	
	int nGroups = 4;			// number of synfire chain groups
	int nNeur = 250;			// number of neurons
	int nNeurExc = 0.8*nNeur;	// number of excitatory neurons
	int nNeurInh = 0.2*nNeur;	// number of inhibitory neurons
	int nSynPerNeur = 60;  		// number of synpases per neuron
	int maxDelay = 20;      	// maximal conduction delay

	std::vector<int> gExc(nGroups);
	std::vector<int> gInh(nGroups);

	// Create Synfire Chain Groups
	for (int i = 0; i < nGroups; i++) {
		std::string gExcName = "exc" + std::to_string(i);
		std::string gInhName = "inh" + std::to_string(i);
		gExc[i] = sim.createGroup(gExcName.c_str(),nNeurExc,EXCITATORY_NEURON);
		sim.setNeuronParameters(gExc[i], 0.02f, 0.2f, -65.0f, 8.0f);

		gInh[i] = sim.createGroup(gInhName.c_str(),nNeurInh,INHIBITORY_NEURON);
		sim.setNeuronParameters(gInh[i], 0.1f, 0.2f, -65.0f, 2.0f);
	}

	// specify connectivity
	float wtExc = 6.0f;                   // synaptic weight magnitude if pre is exc
	float wtInh = 5.0f;                   // synaptic weight magnitude if pre is inh (no negative sign)
	float wtMax = 10.0f;                  // maximum synaptic weight magnitude
	float pConn = nSynPerNeur*1.0f/nNeur; // connection probability

	// gExc receives input from nSynPerNeur neurons from both gExc and gInh
	// every neuron in gExc should receive ~nSynPerNeur synapses
	for (int i = 0; i < nGroups; i++){
		sim.connect(gExc[i], gExc[i], "random", RangeWeight(0.0f, wtExc, wtMax), pConn, RangeDelay(1,20), RadiusRF(-1), SYN_PLASTIC);
		sim.connect(gInh[i], gExc[i], "random", RangeWeight(0.0f, wtInh, wtMax), pConn, RangeDelay(1,20), RadiusRF(-1), SYN_PLASTIC);

		if(i < nGroups-1){
			sim.connect(gExc[i], gExc[i + 1], "random", RangeWeight(6.0f), 0.3, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);
            sim.connect(gExc[i], gInh[i + 1], "random", RangeWeight(wtExc), pConn, RangeDelay(10,10), RadiusRF(-1), SYN_FIXED);		
		}
	}

	// gInh receives input from nSynPerNeur neurons from gExc, all delays are 1ms, no plasticity
	// every neuron in gInh should receive ~nSynPerNeur synapses

	// enable STDP on all incoming synapses to gExc
	float alphaPlus = 0.1f, tauPlus = 20.0f, alphaMinus = 0.1f, tauMinus = 20.0f;
	for (int i = 0; i < nGroups; i++) {
        sim.setSTP(gExc[i], true, 0.15f, 750.0f, 50.0f);
        sim.setSTP(gExc[i], true, 0.15f, 750.0f, 50.0f);
    }

	// run CUBA mode
	sim.setConductances(false);

	// ---------------- SETUP STATE -------------------
	sim.setupNetwork();
	std::vector<SpikeMonitor*> SMexc(nGroups);
    std::vector<SpikeMonitor*> SMinh(nGroups);

	std::vector<ConnectionMonitor*> CMee(nGroups);
    std::vector<ConnectionMonitor*> CMei(nGroups);
    for (int i = 0; i < nGroups; i++) {
        SMexc[i] = sim.setSpikeMonitor(gExc[i], "DEFAULT");
        SMinh[i] = sim.setSpikeMonitor(gInh[i], "DEFAULT");
		CMee[i] = sim.setConnectionMonitor(gExc[i], gExc[i], "DEFAULT");
		CMei[i] = sim.setConnectionMonitor(gInh[i], gExc[i], "DEFAULT");
    }

    // ---------------- RUN STATE -------------------
    for (int t = 0; t < 20; t++) {
        std::vector<float> thalamCurrExc(nNeurExc, 0.0f);
        std::vector<float> thalamCurrInh(nNeurInh, 0.0f);
		int randNeurId = floor(drand48()*(nNeur-1) + 0.5);
		float thCurr = 20.0f;
		if (randNeurId < nNeurExc) {
			// neurId belongs to gExc
			thalamCurrExc[randNeurId] = thCurr;
		} else {
			// neurId belongs to gInh
			thalamCurrInh[randNeurId - nNeurExc] = thCurr;
		}
        // Stimulate the first group
        if (t == 0) {
            for (int j = 0; j < nNeurExc; j++) {
                thalamCurrExc[j] = 20.0f;
            }
        }

        sim.setExternalCurrent(gExc[0], thalamCurrExc);
        sim.setExternalCurrent(gInh[0], thalamCurrInh);

        // Run for 10 ms
        sim.runNetwork(0, 500, true);
    }

    // Stop Recording & Print Stats
    for (int i = 0; i < nGroups; i++) {
        SMexc[i]->stopRecording();
        SMinh[i]->stopRecording();
        //SMexc[i]->print(false);
        //SMinh[i]->print(false);
    }

	return 0;
}
