/*! \brief An example of using the PTI interface to tune the initial weigh tranges of an SNN to produce a target 
 *   average firing rate.
 * 
 *  We define an SNN with a single excitatory group, a single inhibitory group, and a group of Poisson inputs.
 *  There are four groups of connections: two that connect the Poisson-spiking inputs to the internal groups 
 *  (Poisson—>excitatory, Poisson—>inhibitory), and two for intra-group connections (excitatory—>excitatory, 
 *  inhibitory, inhibitory).
 * 
 *  The model exposes four tunable parameters, one for each connection group. The aim is to execute many models 
 *  at once in CARLsim with different parameter configuration (ex. to run them all in parallel on the same 
 *  GPU), and to output their fitness values.
 * 
 *  The binary this file reduces too takes comma-delimited parameter lists on std::cin and outputs scalar fitness 
 *  values to std::cout (which are a function of the difference between the observed and target average firing rates).
 *
 */
//! [includes]
#include "PTI.h"
#include <carlsim.h>
#include <iostream>
//! [includes]
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>

using namespace std;

//! [experiment1]
class TuneFiringRatesExperiment : public Experiment {
public:
	// Various constants are defined here at the top that we'll use  later on
//! [experiment1]
	// Decay constants
	const float COND_tAMPA=5.0, COND_tNMDA=150.0, COND_tGABAa=6.0, COND_tGABAb=150.0;
	
	// Number of neurons in each group
	static const int NUM_NEURONS_PER_GROUP = 100;

	// Simulation time in seconds
	//static const int runTime = 2;
	static const int runTime = 10;

	// Target rates for the objective function
	const float INPUT_TARGET_HZ = 30.0f;
	const float EXC_TARGET_HZ   = 10.0f;
	const float INH_TARGET_HZ   = 20.0f;

//! [experiment2]
    const LoggerMode verbosity;
	const SimMode simMode;

	/*!
	 * Set up an experiment object for a given architecture and logging level.
	 */
	TuneFiringRatesExperiment(const SimMode simMode, const LoggerMode verbosity): simMode(simMode), verbosity(verbosity) {}
//! [experiment2]

//! [experiment3]
	void run(const ParameterInstances &parameters, std::ostream &outputStream) const {

		// Construct a CARLsim network on the heap.
		CARLsim* const network = new CARLsim("tuneFiringRates", simMode, verbosity);
//! [experiment3]


//! [experiment4]
		// Define constant Izhikevich parameters for two types of neurons
		const float REG_IZH[] = { 0.02f, 0.2f, -65.0f, 8.0f };
		const float FAST_IZH[] = { 0.1f, 0.2f, -65.0f, 2.0f };

		// The number of individuals (separate parameter configurations) we have received
//#ifdef UNIX
		int indiNum = parameters.getNumInstances();
//#else
//		const int indiNum = 20;
//#endif
//		// Groups for each individual
//		int poissonGroup[indiNum];
//		int excGroup[indiNum];
//		int inhGroup[indiNum];
//
//		// Measure spiking activity on each exc and inh group
//		SpikeMonitor* excMonitor[indiNum];
//		SpikeMonitor* inhMonitor[indiNum];
////! [experiment4]
//
////! [experiment5]
//		// We'll process the spiking activity into a fitness value
//		float excHz[indiNum];
//		float inhHz[indiNum];
//		float excError[indiNum];
//		float inhError[indiNum];
//		float fitness[indiNum];
////! [experiment5]

		// Groups for each individual
		vector<int> poissonGroup(indiNum);
		vector<int> excGroup(indiNum);
		vector<int> inhGroup(indiNum);

		// Measure spiking activity on each exc and inh group
		vector <SpikeMonitor*> excMonitor(indiNum);
		vector <SpikeMonitor*> inhMonitor(indiNum);
		//! [experiment4]

		//! [experiment5]
				// We'll process the spiking activity into a fitness value
		vector<float> excHz(indiNum);
		vector<float> inhHz(indiNum);
		vector<float> excError(indiNum);
		vector<float> inhError(indiNum);
		vector<float> fitness(indiNum);
		//! [experiment5]


//! [experiment6]
		// We'll add groups for *all* the individuals to the same large CARLsim network object.
		// This allows us to run multiple networks side-by-side on the same GPU: we treat them as
		// a single mega-network with many non-interacting components.
		assert(parameters.getNumParameters() >= 4);

		for(unsigned int i = 0; i < parameters.getNumInstances(); i++) {
			/** Decode a genome*/
			poissonGroup[i] = network->createSpikeGeneratorGroup("poisson", NUM_NEURONS_PER_GROUP, EXCITATORY_NEURON);
			excGroup[i] = network->createGroup("exc", NUM_NEURONS_PER_GROUP, EXCITATORY_NEURON);
			inhGroup[i] = network->createGroup("inh", NUM_NEURONS_PER_GROUP, INHIBITORY_NEURON);

			network->setNeuronParameters(excGroup[i], REG_IZH[0], REG_IZH[1], REG_IZH[2], REG_IZH[3]);
			network->setNeuronParameters(inhGroup[i], FAST_IZH[0], FAST_IZH[1], FAST_IZH[2], FAST_IZH[3]);
			network->setConductances(true,COND_tAMPA,COND_tNMDA,COND_tGABAa,COND_tGABAb);

			network->connect(poissonGroup[i], excGroup[i], "random", RangeWeight(parameters.getParameter(i,0)), 0.5f, RangeDelay(1));
			network->connect(excGroup[i], excGroup[i], "random", RangeWeight(parameters.getParameter(i,1)), 0.5f, RangeDelay(1));
			network->connect(excGroup[i], inhGroup[i], "random", RangeWeight(parameters.getParameter(i,2)), 0.5f, RangeDelay(1));
			network->connect(inhGroup[i], excGroup[i], "random", RangeWeight(parameters.getParameter(i,3)), 0.5f, RangeDelay(1));
		}
//! [experiment6]

//! [experiment7]
		// With all the groups and connections specified, we can now setup the mega-network
		network->setupNetwork();
//! [experiment7]

//! [experiment8]
		// Configure the spiking rate for the Poisson inputs
		PoissonRate* const in = new PoissonRate(NUM_NEURONS_PER_GROUP);
		in->setRates(INPUT_TARGET_HZ);

		// Assign the spiking rate and spikeMonitors for each sub-network
		for(unsigned int i = 0; i < parameters.getNumInstances(); i++) {
			network->setSpikeRate(poissonGroup[i],in);

			excMonitor[i] = network->setSpikeMonitor(excGroup[i], "NULL");
			inhMonitor[i] = network->setSpikeMonitor(inhGroup[i], "NULL");

			excMonitor[i]->startRecording();
			inhMonitor[i]->startRecording();

			// initialize all the error and fitness variables
			excHz[i]=0; inhHz[i]=0;
			excError[i]=0; inhError[i]=0;
			fitness[i]=0;
		}
//! [experiment8]

//! [experiment9]
		// GO!
		network->runNetwork(runTime,0);
//! [experiment9]

//! [experiment10]



		// For each sub-network, extract the mean firing rate and compute a fitness value based on its difference from the target rate
		for(unsigned int i = 0; i < parameters.getNumInstances(); i++) {

			excMonitor[i]->stopRecording();
			inhMonitor[i]->stopRecording();

			excHz[i] = excMonitor[i]->getPopMeanFiringRate();
			inhHz[i] = inhMonitor[i]->getPopMeanFiringRate();

			excError[i] = fabs(excHz[i] - EXC_TARGET_HZ);
			inhError[i] = fabs(inhHz[i] - INH_TARGET_HZ);

			fitness[i] = 1/(excError[i] + inhError[i]);
			outputStream << fitness[i] << endl;

		}
//! [experiment10]

//! [experiment11]
		delete network;
		delete in;
	}
};

//! [experiment11]

/*! Some poor-man's CLI parsing: teturns true iff the command-line arguments contain "-parameter". */
// then use Boost to become rich
const bool hasOpt(int argc, const char * const argv[], const char * const parameter) {
  assert(argc >= 0);
  assert(argv != NULL);
  assert(parameter != NULL);

  for (int i = 1; i < argc; i++) {
#ifdef UNIX
	  char dashParam[strlen(parameter) + 1];
#else
	  char dashParam[512];
#endif
    strcpy(dashParam, "-");
    strcat(dashParam, parameter);
    if (0 == strcmp(dashParam, argv[i]))
         return true;
  }
  return false;
}

//! [main1]
int main(int argc, char* argv[]) {
	/* First we Initialize an Experiment and a PTI object.  The PTI parses CLI
	* arguments, and then loads the Parameters from a file (if one has been
	* specified by the user) or else from a default istream (std::cin here). */

  	const SimMode simMode = hasOpt(argc, argv, "cpu") ? CPU_MODE : GPU_MODE;
  	const LoggerMode verbosity = hasOpt(argc, argv, "v") ? USER : SILENT;
//! [main1]
//! [main2]
	const TuneFiringRatesExperiment experiment(simMode, verbosity);
//! [main2]
//! [main3]
	const PTI pti(argc, argv, std::cout, std::cin);

	/* The PTI will now cheerfully iterate through all the Parameter sets and
	* run your Experiment on it, printing the results to the specified
	* ostream (std::cout here). */
	pti.runExperiment(experiment);

	return 0;
}
//! [main3]
