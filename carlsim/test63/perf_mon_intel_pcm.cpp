/* * Copyright (c) 2016 Regents of the University of California. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. The names of its contributors may not be used to endorse or promote
*    products derived from this software without specific prior written
*    permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* *********************************************************************************************** *
* CARLsim
* created by: (MDR) Micah Richert, (JN) Jayram M. Nageswaran
* maintained by:
* (MA) Mike Avery <averym@uci.edu>
* (MB) Michael Beyeler <mbeyeler@uci.edu>,
* (KDC) Kristofor Carlson <kdcarlso@uci.edu>
* (TSC) Ting-Shuo Chou <tingshuc@uci.edu>
* (HK) Hirak J Kashyap <kashyaph@uci.edu>
*
* CARLsim v1.0: JM, MDR
* CARLsim v2.0/v2.1/v2.2: JM, MDR, MA, MB, KDC
* CARLsim3: MB, KDC, TSC
* CARLsim4: TSC, HK
* CARLsim5: HK, JX, KC
* CARLsim6: LN, JX, KC, KW
*
* CARLsim available from http://socsci.uci.edu/~jkrichma/CARLsim/
* Ver 12/31/2016
*/
#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>
#include <periodic_spikegen.h>
#include <snn_definitions.h> // MAX_GRP_PER_SNN


//  https://learn.microsoft.com/en-us/windows/win32/api/pdh/nf-pdh-pdhgetformattedcounterarraya

//#include <windows.h>
#include <stdio.h>

// #ifdef INTEL_PCM

//#include <pcm-lib.h>
#include <cpucounters.h>
#include <pci.h>
#include <utils.h>

using namespace pcm;

TEST(PcmMon, Init) {

	PCM* pcm = PCM::getInstance();
	PCM::ErrorCode status = pcm->program();

	if (status != PCM::Success) {

		//std::cerr << "Intel PCM couldn't start: " << PCM::getErrorCodeString(status) << std::endl;
		//return 1;

		pcm->checkStatus(status);
	}

	printf("PCM instance %xllu \n", pcm); 

	pcm->cleanup();

}

TEST(PcmMon, pcmCopilot) {

	PCM* pcm = PCM::getInstance();
	PCM::ErrorCode status = pcm->program();
	if (status != PCM::Success) {
		pcm->checkStatus(status);
	}

	// Take initial snapshot
	SystemCounterState before_sstate = getSystemCounterState();
	std::vector<CoreCounterState> before_core_states;
	int num_cores = pcm->getNumCores();
	for (int i = 0; i < num_cores; ++i)
		before_core_states.push_back(getCoreCounterState(i));

	// Sleep for a second (measurement interval)
#ifdef _WIN32
	Sleep(1000);
#else
	sleep(1);
#endif

	// Take final snapshot
	SystemCounterState after_sstate = getSystemCounterState();
	std::vector<CoreCounterState> after_core_states;
	for (int i = 0; i < num_cores; ++i)
		after_core_states.push_back(getCoreCounterState(i));

	// Print per-core metrics

	printf("Core  UTIL  IPC  FREQ(GHz)\n");


	for (int i = 0; i < num_cores; ++i) {
		//double util = getCoreActiveRatio(before_core_states[i], after_core_states[i]) * 100.0; // %
		//double util = getExecUsage(before_core_states[i], after_core_states[i]) * 100.0; // %		
		//double util = getExecUsage(before_core_states[i], after_core_states[i]); // UTIL  : utlization (same as core C0 state active state residency, the value is in 0..1)		
		double util = getCoreCStateResidency(0, before_core_states[i], after_core_states[i]); // UTIL  : utlization (same as core C0 state active state residency, the value is in 0..1)				
		double ipc = getIPC(before_core_states[i], after_core_states[i]); // instructions per CPU cycle
		double freq = getAverageFrequency(before_core_states[i], after_core_states[i]) / 1000000000.; // GHz

		printf("%2d    %1.2f  %1.2f   %1.3f\n", i, util, ipc, freq);

		//std::cout << "Core " << i << ": "
		//	<< "UTIL=" << util << "%, "
		//	<< "IPC=" << ipc << ", "
		//	<< "FREQ=" << freq << " GHz"
		//	<< std::endl;

		 
	}

	pcm->cleanup();

}