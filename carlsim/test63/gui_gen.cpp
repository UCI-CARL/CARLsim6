

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>
#include "normal_spikegen.h"
#include "file_connection_generator.h"
#include <snn_definitions.h> // MAX_GRP_PER_SNN

#include <cmath>
#include <vector>
#include <array>



TEST(CsGen, mini) {

	// cd csgen

	CARLsim* carlsim = new CARLsim("netName", CPU_MODE, USER, 0, 42);


	// include  "create_spikegen.h"
	NormalSpikeGenerator* spike_gen_0 = new NormalSpikeGenerator(10.000000, 2.400000, 400, 200);

	// CONFIG STATE
	carlsim->setIntegrationMethod(RUNGE_KUTTA4, 10);


	int grpId_0;
	{
		auto grpId = carlsim->createSpikeGeneratorGroup("Cstim", 200, EXCITATORY_NEURON);
		carlsim->setSpikeGenerator(grpId, spike_gen_0);
		assert(grpId == 0);
		grpId_0 = grpId;
	}
	int grpId_1;
	{
		auto grpId = carlsim->createGroup("Cexc0", 200, EXCITATORY_NEURON, 0, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 1);
		grpId_1 = grpId;
	}

	int conn_id_0;
	ConnectionGeneratorFromFile* conngen_0 = nullptr;
	{
		conngen_0 = new ConnectionGeneratorFromFile("csgen\\conngrpgen_0_0_1.dat");
		auto connId = carlsim->connect(0, 1, conngen_0, SYN_FIXED);
		assert(connId == 0);
		conn_id_0 = connId;
	}

	carlsim->setupNetwork();

	delete conngen_0;


	// include "delete_spikegen.h"
	//delete spike_gen_0;


	carlsim->runNetwork(3, 0, true);

	delete carlsim;

	EXPECT_TRUE(true);
}




TEST(CsGen, synfire4) {

/*
	std::vector<float> vect(200, .0f);
	//std::vector<std::pair<int, float>> aer = {};
	std::vector<std::pair<int, float>> aer = { {3,80.000000}, {69,80.000000}, {130,80.000000} };
	for (auto iter = aer.begin(); iter != aer.end(); iter++) { vect[iter->first] = iter->second; };
*/


	// cd csgen

	CARLsim* carlsim = new CARLsim("synfire4loopB", CPU_MODE, USER, 0, 42);

	// CONFIG STATE
	carlsim->setIntegrationMethod(RUNGE_KUTTA4, 10);


	#include "synfire4loopB/generators.h"

	#include "synfire4loopB/groups.h"

	#include "synfire4loopB/connections.h"
	
	//auto spikemon_0 = carlsim->setSpikeMonitor(0, "DEFAULT");
	//spikemon_0->setPersistentData(true);

	#include "synfire4loopB/monitors.h"


	carlsim->setupNetwork();


	// include "delete_spikegen.h"
	//delete spike_gen_0;


	for (int i = 0; i < 200; i++) {
		carlsim->runNetwork(0, 1, false);
	}

	
	for (int i = 0; i < 1500; i++) {
		carlsim->runNetwork(0, 1, false);
	}

	//for (int i = 0; i < 5; i++) {
	//	carlsim->runNetwork(0, 100, true);
	//}

	//for (int i = 0; i < 3; i++) {
	//	carlsim->runNetwork(1, 0, true);
	//}

	#include "synfire4loopB/deletes.h"

	delete carlsim;

	EXPECT_TRUE(true);
}



TEST(CsGen, synfireMin) {

	
	// cd csgen

	CARLsim* carlsim = new CARLsim("synfireMin", CPU_MODE, USER, 0, 42);

	// CONFIG STATE
	carlsim->setIntegrationMethod(RUNGE_KUTTA4, 10);


#include "synfireMin/generators.h"

#include "synfireMin/groups.h"

#include "synfireMin/connections.h"

		//auto spikemon_0 = carlsim->setSpikeMonitor(0, "DEFAULT");
		//spikemon_0->setPersistentData(true);

#include "synfireMin/monitors.h"


	carlsim->setupNetwork();


	// include "delete_spikegen.h"
	//delete spike_gen_0;


	for (int i = 0; i < 200; i++) {
		carlsim->runNetwork(0, 1, false);
	}


	//for (int i = 0; i < 1500; i++) {
	//	carlsim->runNetwork(0, 1, false);
	//}

	//for (int i = 0; i < 5; i++) {
	//	carlsim->runNetwork(0, 100, true);
	//}

	//for (int i = 0; i < 3; i++) {
	//	carlsim->runNetwork(1, 0, true);
	//}

#include "synfireMin/deletes.h"

	delete carlsim;

	EXPECT_TRUE(true);
}

