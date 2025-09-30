

#include <carlsim.h>
#include "normal_spikegen2.h"
//#include "normal_spikegen.h"
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
	carlsim->setIntegrationMethod(RUNGE_KUTTA4, 10);


#include "generators.h"

#include "groups.h"

#include "connections.h"

	//auto spikemon_0 = carlsim->setSpikeMonitor(0, "DEFAULT");
	//spikemon_0->setPersistentData(true);

#include "monitors.h"


	carlsim->setupNetwork();


	// include "delete_spikegen.h"
	//delete spike_gen_0;

	if (T == -1)
		T = 3;  // sec

	//t in ms
	for (int t = 0; t < T * 10 * 5 * 20; t++) {
	//for (int t = 0; t < T * 10 * 10 * 5 ; t++) {

		//bool bSpikeMon = false;
		//bool bPerfMon = false;

		//carlsim->runNetwork(0, 100, false);
		//carlsim->runNetwork(0, 20, false);
		carlsim->runNetwork(0, 1, false);
		//carlsim->runNetwork(0, 2, false);

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

#include "deletes.h"

	delete carlsim;


}