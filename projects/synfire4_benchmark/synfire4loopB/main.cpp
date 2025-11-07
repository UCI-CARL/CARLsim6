

#include <carlsim.h>
//#include "normal_spikegen2.h"
#include "normal_spikegen.h"
#include "file_connection_generator.h"
#include <snn_definitions.h> // MAX_GRP_PER_SNN

#include <cmath>
#include <vector>
#include <array>
#include <assert.h>

#include <cstdlib>


int main(int argc, const char* argv[]) {

	CARLsim::InitParams(argc, argv, "carlsim.conf");

	int partitions = CARLsim::Params()[CUSTOM_1_PARAM];

	int T = CARLsim::Params()[CUSTOM_2_PARAM];


	// Instanciate the CARLsim simulation object
	CARLsim* carlsim = new CARLsim("synfire4loopB", (SimMode)0, (LoggerMode)0, 0, 42);

	// CONFIG STATE
	//carlsim->setIntegrationMethod(RUNGE_KUTTA4, 10);   // Deadw(9) -> 19.6%

	// ISCAS 2026
	//carlsim->setIntegrationMethod(FORWARD_EULER, 4);
	//carlsim->setIntegrationMethod(FORWARD_EULER, 8);
	//carlsim->setIntegrationMethod(FORWARD_EULER, 10);
	//carlsim->setIntegrationMethod(FORWARD_EULER, 20); // Deadw(9): 25% 
	carlsim->setIntegrationMethod(FORWARD_EULER, 25); // Daedw(9): 20.6% BINGO  -> 56% (8) ->  2.8 x
	//carlsim->setIntegrationMethod(FORWARD_EULER, 30); // Deadw(9): 17%
	//carlsim->setIntegrationMethod(FORWARD_EULER, 40); // Deadw(9): 13%
	//carlsim->setIntegrationMethod(FORWARD_EULER, 100);


	//#include "generators.h"
	//NormalSpikeGenerator* spikegen_0 = new NormalSpikeGenerator(10.000000, 2.400000, 400, 200);
	NormalSpikeGenerator* spikegen_0 = new NormalSpikeGenerator(10.000000, 2.400000, 400, 0);

//#include "groups.h"
//#include "groups10.h"
//#include "groups-1part-cuba.h"
#include "groups-1part-coba.h"  
	 
	//const int Deadweight = 0;
	//const int Deadweight = 1;  // 2.1 -> 3.0 (2) => 1.42 (42%)  FE4: 3.0 -> 3.8 => 1.23 (23%) FE10:  1.8 -> 2.6(4) => 1.4 
	////const int Deadweight = 2;  // 1.6 -> 2.6 (4) => 1.6 x (60%)
	////const int Deadweight = 4;  //  0.9 -> 1.9 (8) => 2.1 x (110%)   FE 10  COBA ???
	const int Deadweight = 9;  //  FE4: 1.0 -> 2.1(4)  => 2.1 (110%)

	if (Deadweight > 0) {
		int grpId_9;
		{
			auto grpId = carlsim->createGroup("CexcN", 800 * Deadweight, EXCITATORY_NEURON, 0, (ComputingBackend)0);
			carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
			carlsim->setConductances(grpId, false);
			assert(grpId == 9);
			grpId_9 = grpId;
		}

		int grpId_10;
		{
			auto grpId = carlsim->createGroup("CinhN", 200 * Deadweight, INHIBITORY_NEURON, 0, (ComputingBackend)0);
			carlsim->setNeuronParameters(grpId, 0.100000, 0.200000, -65.000000, 2.000000);
			carlsim->setConductances(grpId, false);
			assert(grpId == 10);
			grpId_10 = grpId;
		}
	}

#include "connections-file.h" // load connections from binary file
//#include "connections-random.h" // generate random connections

	//auto spikemon_0 = carlsim->setSpikeMonitor(0, "DEFAULT");
	//spikemon_0->setPersistentData(true);

//#include "monitors.h"

	carlsim->setupNetwork();

	// include "delete_spikegen.h"
	//delete spike_gen_0;

	if (T == -1)
		T = 3;  // s

	//t in ms, T in s
	for (int t = 0; t < T * 10 * 5; t++) {  
		carlsim->runNetwork(0, 20, false);
	}

	//for (int t = 0; t < T * 10 * 5 * 20; t++) {
	//	bool bSpikeMon = false;
	//	bool bPerfMon = false;
	//	carlsim->runNetwork(0, 100, false);
	//	carlsim->runNetwork(0, 1, false);
	//	carlsim->runNetwork(0, 2, false);
	//}

	//for (int i = 0; i < 1500; i++) {
	//	carlsim->runNetwork(0, 1, false);
	//}

	//for (int i = 0; i < 5; i++) {
	//	carlsim->runNetwork(0, 100, true);
	//}

	//for (int i = 0; i < 3; i++) {
	//	carlsim->runNetwork(1, 0, true);
	//}

#include "deletes.h"

	delete carlsim;

}