
#ifdef LN_I_CALC_TYPES

#define SPK_MON

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>

#include <periodic_spikegen.h>
#include <spikegen_from_vector.h>
#include <pre_post_group_spikegen.h>

#include <vector>

const int TRACE_Fig1b = 1;  // 1 ERROR, 2 INFO, 3 DEBUG, 4 TRACE
const bool CHECK_Fig1b_SecondConn_CPU = true;
const bool CHECK_Fig1b_SecondConn_GPU = true;
const bool CHECK_Fig1b_GPU_w1 = true;
const bool CHECK_Fig1b_GPU_w2 = true;

/*!
 * \brief PKA PLC controlled LTP/LTD 
 *
 * [Nadim 2014] Fig 1b.
 * 
 *  
 * connect all together build network      
 * 
 */

 /*!
  * \brief testing the exponential E-STDP curve
  * 
  * This function tests whether E-STDP change synaptic weight as expected
  * The ALPHA_LTD_a,..,g are the control groups (reference) 
  * 
  * With control of pre- and post-neurons' spikes, the synaptic weight is expected to increase or decrease to
  * maximum or minimum synaptic weight respectively.
  */
TEST(PKA_PLC_MOD, Fig1b) {
	// simulation details
	int size;
	int gex1, gex2, g1, g2;
	float ALPHA_LTP = 0.08f;
	float ALPHA_LTD = -0.12f;
	float TAU_LTP = 8.0f;
	float TAU_LTD = 8.0f;
	float maxInhWeight = 10.0f;
	float initWeight = 5.0f;
	float minInhWeight = 0.0f;

	int nm_pka = NM_NE;
	int nm_plc = NM_ACh;

	enum FigNr          {   _a,     _b,      _c,     _d,     _e,     _f,    _g     };
	const char *label[] = { "_a", "_b", "_c", "_d", "_e", "_f", "_g" };

	float ne [_g + 1] = {  1.0f,   1.0f,    0.0f,    0.0f,   0.6f,  1.4f,  1.25f   };
	float ach[_g + 1] = {  1.0f,   0.0f,    1.0f,    0.0f,   0.6f,  1.4f,  0.75f   };

	float a_p[_g + 1] = {  0.08f,  0.16f,  -0.08f,  0.00f,  0.048f,  0.112f,  0.14f  };
	float a_m[_g + 1] = { -0.12f,  0.12f,  -0.24f,  0.00f, -0.072f, -0.168f, -0.03f };


	std::vector<std::vector<float>> weights1[TESTED_MODES];
	std::vector<std::vector<float>> weights2[TESTED_MODES];

	LoggerMode logger = SILENT; // USER DEVELOPER;

	for (int fig = _a; fig <= _g; fig++) {
	//{int fig = _a; 	// ok
	//{int fig = _b; 	 // nok  PLC, see TRACE=3  CHECKS=off

		int tested = 0;
		for (int mode = 0; mode < TESTED_MODES; mode++) {
		//{int mode = 0; 
			//for (int coba = 0; coba < 2; coba++) {
			{int coba = 0;
			if (TRACE_Fig1b > 3) printf("fig:%s mode:%d pka:NE=%.1f  plc:ACh=%.1f\n", label[fig], mode, ne[fig], ach[fig]);
				//for (int offset = -30; offset <= 30; offset += 5) {
				for (int offset = -10; offset <= 10; offset += 2) {
					if (offset == 0) continue; // skip offset == 0;
					// create a network
					CARLsim* sim = new CARLsim("STDP.ESTDPExpCurve", mode ? GPU_MODE : CPU_MODE, logger, 1, 42);

					// STDP 
					g1 = sim->createGroup("excit", 1, EXCITATORY_NEURON, -1, mode ? GPU_CORES: CPU_CORES);
					sim->setNeuronParameters(g1, 0.02f, 0.2f, -65.0f, 8.0f);

					// PKA_PLC_MOD STDP
					g2 = sim->createGroup("excit", 1, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
					sim->setNeuronParameters(g2, 0.02f, 0.2f, -65.0f, 8.0f);
					sim->setNeuromodulator(g2, 
						1.0f, 1.0f, 1.0f, false, // DA
						1.0f, 1.0f, 1.0f, false, // 5HT
						ach[fig], 1000000, 0.000001f, true, // hold base line
						ne[fig],  1000000, 0.000001f, true);

					gex1 = sim->createSpikeGeneratorGroup("input-ex1", 1, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
					gex2 = sim->createSpikeGeneratorGroup("input-ex2", 1, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);

					PrePostGroupSpikeGenerator* prePostSpikeGen = new PrePostGroupSpikeGenerator(100, offset, gex2, gex1);

					if (coba) { // conductance-based
						//sim->connect(gex1, g1, "one-to-one", RangeWeight(40.0f / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
						//sim->connect(gex2, g1, "one-to-one", RangeWeight(minInhWeight, initWeight / 100, maxInhWeight / 100), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

						//// enable COBA, set up ESTDP
						//sim->setConductances(true, 5, 150, 6, 150);
						//sim->setESTDP(gex1, g1, true, STANDARD, ExpCurve(ALPHA_LTP / 100, TAU_LTP, ALPHA_LTD / 100, TAU_LTP));
						//sim->setESTDP(gex2, g1, true, STANDARD, ExpCurve(ALPHA_LTP / 100, TAU_LTP, ALPHA_LTD / 100, TAU_LTP));
					}
					else { // current-based
						sim->connect(gex1, g1, "one-to-one", RangeWeight(40.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
						sim->connect(gex2, g1, "one-to-one", RangeWeight(minInhWeight, initWeight, maxInhWeight), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

						sim->connect(gex1, g2, "one-to-one", RangeWeight(40.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
						sim->connect(gex2, g2, "one-to-one", RangeWeight(minInhWeight, initWeight, maxInhWeight), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_PLASTIC);

						// set up ESTDP with reference values from matlab script
						sim->setConductances(false);
						sim->setESTDP(gex2, g1, true, STANDARD, ExpCurve(a_p[fig], TAU_LTP, a_m[fig], TAU_LTP));
						//sim->setESTDP(gex2, g1, true, PKA_PLC_MOD, ExpCurve(ALPHA_LTP, TAU_LTP, ALPHA_LTD, TAU_LTP), PkaPlcModulation(nm_pka, 1.0f, nm_plc, 1.0f));  // test PKA on first conn -> same as reference
						//sim->setESTDP(gex2, g1, true, PKA_PLC_MOD, ExpCurve(ALPHA_LTP, TAU_LTP, ALPHA_LTD, TAU_LTP), PkaPlcModulation(nm_pka, 0.5f, nm_plc, 0.5f));  // test PKA on first conn -> differnt as reference -> nm effective

						// set up PKA_PLC_MOD ESTDP
						sim->setConductances(false);
						////working as reference => Test case valid
						sim->setESTDP(gex2, g2, true, PKA_PLC_MOD, ExpCurve(ALPHA_LTP, TAU_LTP, ALPHA_LTD, TAU_LTP), PkaPlcModulation(nm_pka, 1.0f, nm_plc, 1.0f));

						// LN2021 \todo merge KXN's fix							
						// The issue has nothing to do with PKA_PLC modulation. 
						// Proof: Setting both connections to STDP without modulation produces the same issue. 
						// The issue seems to be caused by the second STPD connection. 
						//sim->setESTDP(gex2, g2, true, STANDARD, ExpCurve(a_p[fig], TAU_LTP, a_m[fig], TAU_LTP));  
					}

					// set up spike controller on DA neurons
					sim->setSpikeGenerator(gex1, prePostSpikeGen);
					sim->setSpikeGenerator(gex2, prePostSpikeGen);


					// build the network
					sim->setupNetwork();

					ConnectionMonitor* CM1 = sim->setConnectionMonitor(gex2, g1, "NULL");
					CM1->setUpdateTimeIntervalSec(-1);
					ConnectionMonitor* CM2 = sim->setConnectionMonitor(gex2, g2, "NULL");
					CM2->setUpdateTimeIntervalSec(-1);
					//SpikeMonitor* SMgex1 = sim->setSpikeMonitor(gex1, "Default");
					//SpikeMonitor* SMgex2 = sim->setSpikeMonitor(gex2, "Default");
					//SpikeMonitor* SMg1 = sim->setSpikeMonitor(g1, "Default");

					//SMgex1->startRecording();
					//SMgex2->startRecording();
					//SMg1->startRecording();

					//sim->runNetwork(55, 0, false);
					//sim->runNetwork(5, 0, false);
					sim->runNetwork(1, 0, false);

					//SMgex1->stopRecording();
					//SMgex2->stopRecording();
					//SMg1->stopRecording();

					//SMgex1->print(true);
					//SMgex2->print(true);
					//SMg1->print(true);

					weights1[mode] = CM1->takeSnapshot();
					weights2[mode] = CM2->takeSnapshot();
					if (offset > 0) { // pre-post
						if (coba) {
							//EXPECT_NEAR(maxInhWeight / 100, weights[0][0], 0.005f);
							////printf("mode:%d coba:%d offset:%d w:%f\n", mode, coba, offset, weights[0][0]);
						}
						else {
							// compare reference against modulated 
							if (CHECK_Fig1b_SecondConn_CPU && mode == 0) EXPECT_NEAR(weights1[mode][0][0], weights2[mode][0][0], 0.5f);
							if (CHECK_Fig1b_SecondConn_GPU && mode == 1) EXPECT_NEAR(weights1[mode][0][0], weights2[mode][0][0], 0.5f);
							if(TRACE_Fig1b > 3) printf("mode:%d coba:%d offset:%d w1:%f w2:%f\n", mode, coba, offset, weights1[mode][0][0], weights2[mode][0][0]);
						}
					}
					else { // post-pre
						if (coba) {
							//EXPECT_NEAR(minInhWeight / 100, weights[0][0], 0.005f);
							////printf("mode:%d coba:%d offset:%d w:%f\n", mode, coba, offset, weights[0][0]);
						}
						else {
							if (CHECK_Fig1b_SecondConn_CPU && mode == 0) EXPECT_NEAR(weights1[mode][0][0], weights2[mode][0][0], 0.5f);
							if (CHECK_Fig1b_SecondConn_GPU && mode == 1) EXPECT_NEAR(weights1[mode][0][0], weights2[mode][0][0], 0.5f);
							if(TRACE_Fig1b > 3) printf("mode:%d coba:%d offset:%d w1:%f w2:%f\n", mode, coba, offset, weights1[mode][0][0], weights2[mode][0][0]);
						}
					}

					delete prePostSpikeGen;
					delete sim;
				}
			}
			tested++;  // CAUTION checks only the last iteration
		}
		// CPU vs GPU
		if (CHECK_Fig1b_GPU_w1 && tested == 2) {  // first (CPU) is reference
			if (TRACE_Fig1b > 2) printf("fig:%s w1_CPU:%f w1_GPU:%f\n", label[fig], weights1[0][0][0], weights1[1][0][0]);
			EXPECT_NEAR(weights1[0][0][0], weights1[1][0][0], 0.1f); ;
		}
		if (CHECK_Fig1b_GPU_w2 && tested == 2) {  // first (CPU) is reference
			if (TRACE_Fig1b > 2) printf("fig:%s w2_CPU:%f w2_GPU:%f\n", label[fig], weights2[0][0][0], weights2[1][0][0]);
			EXPECT_NEAR(															weights2[0][0][0], weights2[1][0][0], 0.1f); ;
		}
	}
}


#endif
