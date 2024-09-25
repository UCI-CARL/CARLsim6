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

//#include <carlsim.h>

#ifndef __NO_CUDA__
#include <cuda_version_control.h>
#else
#include <execution_stopwatch.h>
#endif

#include <thread>

// tests whether the stopwatch functionality used to get the simulation's actual execution time works correctly
TEST(Stopwatch, ExecutionStopwatch) {
	// Create timer object
#ifndef __NO_CUDA__
	StopWatchInterface* timer;
	CUDA_CREATE_TIMER(timer);
#else
	ExecutionStopwatch* timer = new ExecutionStopwatch{};
#endif

	const int durations[] = { 0, 10, 100, 500 }; 
	const int n = 4; 

	for (int i = 0; i < n; ++i) {
		int duration = durations[i];

		// Start timer
#ifndef __NO_CUDA__
		CUDA_START_TIMER(timer);
#else
		timer->start();
#endif

		std::this_thread::sleep_for(std::chrono::milliseconds(duration));

		// Stop timer, get time elapsed, and reset timer
#ifndef __NO_CUDA__
		CUDA_STOP_TIMER(timer);
		float measured_time = CUDA_GET_TIMER_VALUE(timer);
		CUDA_RESET_TIMER(timer);
#else
		timer->stop();
		float measured_time = timer->get_time() * 1000;
		timer->reset();
#endif

		float measured_expected_diff = measured_time - duration;
		EXPECT_LE(1e-6, measured_expected_diff);
	}

#ifndef __NO_CUDA__
	CUDA_DELETE_TIMER(timer);
#else
	delete timer;
#endif
}