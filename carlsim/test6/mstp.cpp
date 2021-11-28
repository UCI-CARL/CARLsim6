
#ifdef LN_I_CALC_TYPES

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>


#include <periodic_spikegen.h>

//#define DEBUG_firingRateSTDvsSTFvsNM4

/*!
 * \brief test the neuromodulation of short-term depression (STD) and short-term facilitation (STF) on post-rate
 *
 * \todo: decribe Nadim testcase, NM4
 * 
 * This test ensures that STD and STF have the expected effect on post-synaptic firing rate.
 * A SpikeGenerator @ 10 Hz is connected to an excitatory post-neuron. First, STP is disabled, and the post-rate
 * is recorded. Then we turn on STD, and expect the firing rate to decrease. Then we turn on STF (instead of STD),
 * and expect the firing rate to increase.
 * However, if the stimulation period is short (isRunLong==0, runTimeMs=10 ms), then the firing rate should not
 * change at all, because the first spike under STP should not make a difference (due to the scaling of STP_A).
 * We perform this procedure in CUBA and COBA mode.
 */
TEST(mSTP, firingRateSTDvsSTFvsNM4) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	int randSeed = rand() % 1000;	// randSeed must not interfere with STP

	CARLsim *sim = NULL;
	SpikeMonitor *spkMonG2 = NULL, *spkMonG3 = NULL;
	PeriodicSpikeGenerator *spkGenG0 = NULL, *spkGenG1 = NULL;

	for (int isRunLong=0; isRunLong<=1; isRunLong++) {
	//int isRunLong = 1;
		for (int hasCOBA=0; hasCOBA<=1; hasCOBA++) {
		//{int hasCOBA = 1;
			for (int mode = 0; mode < TESTED_MODES; mode++) {
			//{int mode = 1;

				// compare
				float rateG2noSTP = -1.0f;
				float rateG3noSTP = -1.0f;

				for (int hasSTP = 0; hasSTP <= 1; hasSTP++) {
				//{int hasSTP = 1;

					for (int hasNM4 = 0; hasNM4 <= 1; hasNM4++) {

						sim = new CARLsim("STP.firingRateSTDvsSTF", mode ? GPU_MODE : CPU_MODE, SILENT, 1, randSeed);
						int g2 = sim->createGroup("STD", 1, EXCITATORY_NEURON, - 1, mode ? GPU_CORES : CPU_CORES);
						int g3 = sim->createGroup("STF", 1, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
						sim->setNeuronParameters(g2, 0.02f, 0.2f, -65.0f, 8.0f);
						sim->setNeuronParameters(g3, 0.02f, 0.2f, -65.0f, 8.0f);
						int g0 = sim->createSpikeGeneratorGroup("input0", 1, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
						int g1 = sim->createSpikeGeneratorGroup("input1", 1, EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);

						float wt = hasCOBA ? 0.2f : 18.0f;
						sim->connect(g0, g2, "full", RangeWeight(wt), 1.0f, RangeDelay(1));
						sim->connect(g1, g3, "full", RangeWeight(wt), 1.0f, RangeDelay(1));

						if (hasCOBA)
							sim->setConductances(true, 5, 0, 150, 6, 0, 150);
						else
							sim->setConductances(false);

						if (hasSTP) {
							float baseDA = 1.0f;
							float base5HT = 1.0f;
							sim->setSTP(g0, true, 0.45f, 50.0f, 750.0f); // depressive
							if(hasNM4) {
								float u[] = { 1.0f, 0.0f, 0.0f, 0.0f, -0.30f / 0.45f, 1.0f };
								float tau_u[] = { 1.0f, 0.0f, 0.0f, 0.0f, 700.0f / 50.0f, 1.0f };
								float tau_x[] = { 1.0f, 0.0f, 0.0f, 0.0f, -700.0f / 750.0f, 1.0f };

								sim->setNM4STP(g0, u, tau_u, tau_x);
								sim->setNeuromodulator(g0,
									baseDA, 1.f, 0.f, true,	// never goes below base
									0.001f, 1.f, 0.f, false,
									0.001f, 1.f, 0.f, false,
									0.001f, 1.f, 0.f, false);
							}
							sim->setSTP(g1, true, 0.15f, 750.0f, 50.0f); // facilitative
							if(hasNM4) {
								float u[] = { 0.0f, 1.0f, 0.0f, 0.0f, 0.30f / 0.15f, 1.0f };
								float tau_u[] = { 0.0f, 1.0f, 0.0f, 0.0f, -700.0f / 750.0f, 1.0f };
								float tau_x[] = { 0.0f, 1.0f, 0.0f, 0.0f, 700.0f / 50.0f, 1.0f };
								sim->setNM4STP(g1, u, tau_u, tau_x);
								sim->setNeuromodulator(g1,
									0.001f, 1.f, 0.f, false,
									base5HT, 1.f, 0.f, true,
									0.001f, 1.f, 0.f, false,
									0.001f, 1.f, 0.f, false);
							}
						}

						bool spikeAtZero = true;
						spkGenG0 = new PeriodicSpikeGenerator(10.0f, spikeAtZero); // periodic spiking @ 15 Hz
						sim->setSpikeGenerator(g0, spkGenG0);
						spkGenG1 = new PeriodicSpikeGenerator(10.0f, spikeAtZero); // periodic spiking @ 15 Hz
						sim->setSpikeGenerator(g1, spkGenG1);

						sim->setupNetwork();

						sim->setSpikeMonitor(g0, "NULL");
						sim->setSpikeMonitor(g1, "NULL");
						spkMonG2 = sim->setSpikeMonitor(g2, "NULL");
						spkMonG3 = sim->setSpikeMonitor(g3, "NULL");

						spkMonG2->startRecording();
						spkMonG3->startRecording();
						int runTimeMs = isRunLong ? 2000 : 100;
						sim->runNetwork(runTimeMs / 1000, runTimeMs % 1000);
						spkMonG2->stopRecording();
						spkMonG3->stopRecording();

						if (!hasSTP) {
							// if STP is off: record spike rate, so that afterwards we can compare it to the one with STP
							// enabled
							spkMonG2->print(true);
							spkMonG3->print(true);
							rateG2noSTP = spkMonG2->getPopMeanFiringRate();
							rateG3noSTP = spkMonG3->getPopMeanFiringRate();
						}
						else {
							spkMonG2->print(true);
							spkMonG3->print(true);
#ifdef DEBUG_firingRateSTDvsSTFvsNM4
							fprintf(stderr, "%s %s %s, G2 w/o=%f, G2 w/%s=%f\n", isRunLong ? "long" : "short",
								mode ? "GPU" : "CPU",
								hasCOBA ? "COBA" : "CUBA",
								rateG2noSTP, 
								hasNM4 ? "nm4" : "", 
								spkMonG2->getPopMeanFiringRate());
							fprintf(stderr, "%s %s %s, G3 w/o=%f, G3 w/%s=%f\n", isRunLong ? "long" : "short",
								mode ? "GPU" : "CPU",
								hasCOBA ? "COBA" : "CUBA",
								rateG3noSTP,
								hasNM4 ? "nm4" : "",
								spkMonG3->getPopMeanFiringRate());
#endif
							// if STP is on: compare spike rate to the one recorded without STP
							if (isRunLong) {
								if (!hasNM4) {
									// the run time was relatively long, so STP should have its expected effect
									EXPECT_TRUE(spkMonG2->getPopMeanFiringRate() < rateG2noSTP); // depressive
									EXPECT_TRUE(spkMonG3->getPopMeanFiringRate() > rateG3noSTP); // facilitative
								}								
								else {
									// the effect is reversed
									EXPECT_TRUE(spkMonG2->getPopMeanFiringRate() > rateG2noSTP); // facilitative due modulation 
									EXPECT_TRUE(spkMonG3->getPopMeanFiringRate() < rateG3noSTP); // depressive due modulation 
								}
							}
							else {
								// the run time was really short, so STP should have no effect (because we scale STP_A so
								// that STP has no weakening/strengthening effect on the first spike)
								EXPECT_FLOAT_EQ(spkMonG2->getPopMeanFiringRate(), rateG2noSTP); // equivalent
								EXPECT_FLOAT_EQ(spkMonG3->getPopMeanFiringRate(), rateG3noSTP); // equivalent
							}
						}

						delete spkGenG0, spkGenG1;
						delete sim;
					}
				}
			}
		}
	}
}

#endif