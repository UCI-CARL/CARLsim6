

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>
#include "normal_spikegen.h"
#include "file_connection_generator.h"
#include <snn_definitions.h> // MAX_GRP_PER_SNN

#include <cmath>
#include <vector>
#include <array>



TEST(CsGen, synfire) {

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
		auto grpId = carlsim->createGroup("Cexc0", 200, EXCITATORY_NEURON, 0, CPU_CORES);
		carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 1);
		grpId_1 = grpId;
	}

	ConnectionGeneratorFromFile* conngen_0 = nullptr;
	int conn_id_0;
	{
		conngen_0 = new ConnectionGeneratorFromFile("csgen\\conngrpgen_0_0_1.dat");
		conn_id_0 = carlsim->connect(0, 1, conngen_0, SYN_FIXED);

		//ConnectionGeneratorFromFile conngen("csgen\\conngrpgen_0_0_1.dat");
		//conn_id_0 = carlsim->connect(0, 1, &conngen, SYN_FIXED);


		assert(conn_id_0 == 0);
	}

	carlsim->setupNetwork();

	delete conngen_0;


	// include "delete_spikegen.h"
	//delete spike_gen_0;


	carlsim->runNetwork(3, 0, true);

	delete carlsim;

	EXPECT_TRUE(true);
}