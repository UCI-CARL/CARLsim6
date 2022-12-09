/*
Test of CSTP processing performance

Concept: 
Test that checks the processing speed of CSTP code meets a certain processing time.
*/

#include "gtest/gtest.h"
#include "carlsim_tests.h"
#include <carlsim.h>

// Include libraries that will allow for us to perform vector operations, and
// print their results
#include <iostream>
#include <algorithm>
#include <vector>
#include <boost/iterator/counting_iterator.hpp>
#include <ctime>
#include <time.h>
#include <cstdlib>

// Create a function that will print out all of the elements of a vector
void print(std::vector <int> const &a) {
 std::cout << "The vector elements are : ";
 for(int i=0; i < a.size(); i++)
		std::cout << a.at(i) << ' ';
}

// Create a function that will create a subset of a vector, which will can be
// used in defining Poisson rates for a fraction of neurons in a group
template<typename T>
std::vector<T> slice(std::vector<T> &v, int m, int n)
{
    std::vector<T> vec;
    std::copy(v.begin() + m, v.begin() + n + 1, std::back_inserter(vec));
    return vec;
}

TEST(CA3, cstp_perf) {   
	clock_t t = clock();
	::testing::FLAGS_gtest_death_test_style = "threadsafe";
	// ---------------- CONFIG STATE -------------------
	// create a network on GPU
	int numGPUs = 0;
	int randSeed = 10;
	CARLsim sim("ca3_snn_GPU", GPU_MODE, USER, numGPUs, randSeed);
	const ComputingBackend BACKEND_CORES = GPU_CORES;
	// include header file that contains generation of groups and their properties
	#include "generateCONFIGStateSTP.h"
	// Set the time constants for the excitatory and inhibitory receptors, and
	// set the method of integration to numerically solve the systems of ODEs
	// involved in the SNN
	sim.setIntegrationMethod(RUNGE_KUTTA4, 5);
	// Setting simulation mode to COBA because conductances are needed for the tm synapse model
	sim.setConductances(true);
	// ---------------- SETUP STATE -------------------
	// build the network
	sim.setupNetwork();
	// Declare variables that will store the start and end ID for the neurons
	// in the pyramidal group
	int pyr_start = sim.getGroupStartNeuronId(CA3_Pyramidal);
	//std::cout << "Beginning neuron ID for Pyramidal Cells is : " << pyr_start << "\n";
	int pyr_end = sim.getGroupEndNeuronId(CA3_Pyramidal);
	//std::cout << "Ending neuron ID for Pyramidal Cells is : " << pyr_end << "\n";
	int pyr_range = (pyr_end - pyr_start) + 1;
	//std::cout << "The range for Pyramidal Cells is : " << pyr_range << "\n";
	// Create vectors that are the length of the number of neurons in the pyramidal
	// group, and another that will store the current at the position for the
  	// random pyramidal cells that will be selected
	std::vector<int> pyr_vec( boost::counting_iterator<int>( 0 ),
													 boost::counting_iterator<int>( pyr_range ));
  	std::vector<float> current(pyr_range, 0.0f);
	// include header file that contains generation of groups and their properties
	#include "generateSETUPStateSTP.h"
	// Define the number of neurons to receive input from the external current
	int numPyramidalFire = 10000;
	// Set the seed of the pseudo-random number generator based on the current system time
	std::srand(std::time(nullptr));
	// Set external current for a fraction of pyramidal cells based on the random seed
	for (int i = 0; i < numPyramidalFire; i++)
	{
	int randPyrCell = pyr_vec.front() + ( std::rand() % ( pyr_vec.back() - pyr_vec.front() ) );
	current.at(randPyrCell) = 0.000035f;
	}
	// ---------------- RUN STATE -------------------
	// run for a total of 10 seconds in 500ms bins
	for (int i = 0; i < 9*2; i++) {
		if (i == 0) {
			//// alternative input from DG
			sim.setExternalCurrent(CA3_Pyramidal, 40.0f);
			sim.setExternalCurrent(CA3_Axo_Axonic, 100.0f);
			sim.setExternalCurrent(CA3_BC_CCK, 135.0f);
			sim.setExternalCurrent(CA3_Basket, 440.0f); 
			sim.setExternalCurrent(CA3_Bistratified, 135.0f);
			sim.setExternalCurrent(CA3_Ivy, 500.0f);
			sim.setExternalCurrent(CA3_QuadD_LM, 200.0f);
			sim.setExternalCurrent(CA3_MFA_ORDEN, 250.0f);

			sim.runNetwork(0, 500, false);
		} 
		else if (i > 0) {			
			sim.runNetwork(0, 500, false);
		}
	}
	t = clock() - t;

	// Test that the simulation ran in a desired amount of time (less than 20 sec)
	EXPECT_LT(((float)t)/CLOCKS_PER_SEC, 20); 
}
