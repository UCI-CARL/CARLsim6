/*
Tau_d effect specific to each synapse connection

Concept: 
Test that each synapse connection in connection-specific short-term placticity (CSTP)
is multiplied by its own tau_d. Also that the tau_d of one connection does not affect
the current decay rate of another connection.
*/

#include "gtest/gtest.h"
#include "carlsim_tests.h"
#include <carlsim.h>

TEST(CA3, cstp_taud) {   
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	/* Test 1: One Synapse Connection */

	// ---------------- CONFIG STATE -------------------
	LoggerMode logger = SILENT;  // USER, SILENT
	int numGPUs = 0;
	int randSeed = 42;	
	//CARLsim sim("cstp_test_1conn", GPU_MODE, USER, numGPUs, randSeed);
	CARLsim* sim = new CARLsim("cstp_test_1conn", GPU_MODE, logger, numGPUs, randSeed);
	int EC_LI_II_Multipolar_Pyramidal=sim->createGroup("EC_LI_II_Multipolar_Pyramidal", 
	                            1, EXCITATORY_NEURON, ANY, GPU_CORES);  
	int MEC_LII_Stellate=sim->createGroup("MEC_LII_Stellate", 
	                            1, EXCITATORY_NEURON, ANY, GPU_CORES);	
	sim->setNeuronParameters(EC_LI_II_Multipolar_Pyramidal, 204.0f, 0.0f, 0.37f, 0.0f, -70.53f, 0.0f, -39.99f, 
	                                0.0f, 0.001f, 0.0f, 0.01f, 0.0f, 3.96f, 0.0f, -54.95f, 0.0f, 
	                                7.0f, 0.0f, 1); // C,k,vr,vt,a,b,vpeak,c,d
	sim->setNeuronParameters(MEC_LII_Stellate, 118.0f, 0.0f, 0.98f, 0.0f, -58.53f, 0.0f, -43.52f, 
	                                0.0f, 0.004f, 0.0f, 7.0f, 0.0f, 7.85f, 0.0f, -52.68f, 0.0f, 
	                                65.0f, 0.0f, 1);
	/* neuron connection parameters */
	sim->connect(EC_LI_II_Multipolar_Pyramidal, MEC_LII_Stellate, "one-to-one", 50.0f, 1.0f, 
	    RangeDelay(1), RadiusRF(-1), SYN_FIXED, 1, 1);
	sim->setSTP(EC_LI_II_Multipolar_Pyramidal, MEC_LII_Stellate, true, STPu(0.1513, 0.0f),
	                                     STPtauU(69.11, 0.0f),
	                                     STPtauX(98.1, 0.0f),
	                                     STPtdAMPA(6.0, 0.0f),
	                                     STPtdNMDA(150.0, 0.0f),
	                                     STPtdGABAa(5.0, 0.0f),
	                                     STPtdGABAb(150.0, 0.0f),
	                                     STPtrNMDA(0.0f, 0.0f),
	                                     STPtrGABAb(0.0f, 0.0f));
	// ---------------- SETUP STATE -------------------
	sim->setupNetwork();
	SpikeMonitor* SpkMon = sim->setSpikeMonitor(MEC_LII_Stellate,"DEFAULT");
	// ---------------- RUN STATE -------------------
	sim->setExternalCurrent(EC_LI_II_Multipolar_Pyramidal, 100);
	SpkMon->startRecording();
	for (int i=0; i<10; i++) {sim->runNetwork(0,100, true);}
	SpkMon->stopRecording();
	float firing_rate_1conn = SpkMon->getPopMeanFiringRate();
	SpkMon->print(false);
	//printf("firing_rate_1conn:%f\n",firing_rate_1conn);	

	delete sim;

	/* Test 2: Two Synapse Connections */	

	CARLsim* sim2 = new CARLsim("cstp_test_2conn", GPU_MODE, logger, numGPUs, randSeed);
	EC_LI_II_Multipolar_Pyramidal=sim2->createGroup("EC_LI_II_Multipolar_Pyramidal", 
	                            1, EXCITATORY_NEURON, ANY, GPU_CORES);  
	int EC_LI_II_Multipolar_Pyramidal2=sim2->createGroup("EC_LI_II_Multipolar_Pyramidal2", 
                            	1, EXCITATORY_NEURON, ANY, GPU_CORES);
	MEC_LII_Stellate=sim2->createGroup("MEC_LII_Stellate", 
	                            1, EXCITATORY_NEURON, ANY, GPU_CORES);	
	sim2->setNeuronParameters(EC_LI_II_Multipolar_Pyramidal, 204.0f, 0.0f, 0.37f, 0.0f, -70.53f, 0.0f, -39.99f, 
	                                0.0f, 0.001f, 0.0f, 0.01f, 0.0f, 3.96f, 0.0f, -54.95f, 0.0f, 
	                                7.0f, 0.0f, 1);
	sim2->setNeuronParameters(EC_LI_II_Multipolar_Pyramidal2, 204.0f, 0.0f, 0.37f, 0.0f, -70.53f, 0.0f, -39.99f, 
	                                0.0f, 0.001f, 0.0f, 0.01f, 0.0f, 3.96f, 0.0f, -54.95f, 0.0f, 
	                                7.0f, 0.0f, 1);
	sim2->setNeuronParameters(MEC_LII_Stellate, 118.0f, 0.0f, 0.98f, 0.0f, -58.53f, 0.0f, -43.52f, 
	                                0.0f, 0.004f, 0.0f, 7.0f, 0.0f, 7.85f, 0.0f, -52.68f, 0.0f, 
	                                65.0f, 0.0f, 1);
	// neuron connection parameters 
	sim2->connect(EC_LI_II_Multipolar_Pyramidal, MEC_LII_Stellate, "one-to-one", 50.0f, 1.0f, 
	    RangeDelay(1), RadiusRF(-1), SYN_FIXED, 1, 1);
	sim2->connect(EC_LI_II_Multipolar_Pyramidal2, MEC_LII_Stellate, "one-to-one", 50.0f, 1.0f, 
	    RangeDelay(1), RadiusRF(-1), SYN_FIXED, 1, 1);
	sim2->setSTP(EC_LI_II_Multipolar_Pyramidal, MEC_LII_Stellate, true, STPu(0.1513, 0.0f),
	                                     STPtauU(69.11, 0.0f),
	                                     STPtauX(98.1, 0.0f),
	                                     STPtdAMPA(6.0, 0.0f),
	                                     STPtdNMDA(150.0, 0.0f),
	                                     STPtdGABAa(5.0, 0.0f),
	                                     STPtdGABAb(150.0, 0.0f),
	                                     STPtrNMDA(0.0f, 0.0f),
	                                     STPtrGABAb(0.0f, 0.0f));
	sim2->setSTP(EC_LI_II_Multipolar_Pyramidal2, MEC_LII_Stellate, true, STPu(0.1513, 0.0f),
	                                     STPtauU(69.11, 0.0f),
	                                     STPtauX(98.1, 0.0f),
	                                     STPtdAMPA(6.0, 0.0f),
	                                     STPtdNMDA(150.0, 0.0f),
	                                     STPtdGABAa(5.0, 0.0f),
	                                     STPtdGABAb(150.0, 0.0f),
	                                     STPtrNMDA(0.0f, 0.0f),
	                                     STPtrGABAb(0.0f, 0.0f));
	// ---------------- SETUP STATE -------------------
	sim2->setupNetwork();
	SpkMon = sim2->setSpikeMonitor(MEC_LII_Stellate,"DEFAULT");
	// ---------------- RUN STATE -------------------
	sim2->setExternalCurrent(EC_LI_II_Multipolar_Pyramidal, 100);
	sim2->setExternalCurrent(EC_LI_II_Multipolar_Pyramidal2, 0);
	SpkMon->startRecording();
	for (int i=0; i<10; i++) {sim2->runNetwork(0,100, true);}
	SpkMon->stopRecording();
	float firing_rate_2conn = SpkMon->getPopMeanFiringRate();
	SpkMon->print(false);
	//printf("firing_rate_2conn:%f\n",firing_rate_2conn);

	delete sim2;

	// Check that firing rate without an additional connection is equal or less than firing rate 
	// with the connection and 0 external current to the additional connection.
	EXPECT_LE(firing_rate_1conn, firing_rate_2conn); // ms
}
