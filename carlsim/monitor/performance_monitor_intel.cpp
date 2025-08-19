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
* Ver 05/24/2017
*/

#include <performance_monitor_intel.h>

#include <snn.h>				// CARLsim private implementation
#include <snn_definitions.h>	// KERNEL_ERROR, KERNEL_INFO, ...

#include <algorithm>			// std::sort

//#if defined(WIN32) && (__INTEL_PCM__)
using namespace pcm;




PerformanceMonitorIntel::PerformanceMonitorIntel(SNN* snn, int monitorId, int sampleRate):
	PerformanceMonitorCore(snn, monitorId, sampleRate)
{
	pcm_ = nullptr;

	// defer all unsafe operations to init function
	pre_init();
	init();
	post_init();
}

PerformanceMonitorIntel::~PerformanceMonitorIntel() {
	release();
}



// Intel PCM
void PerformanceMonitorIntel::pre_init() {

	PerformanceMonitorCore::pre_init();

	//#if defined(WIN32) && (__INTEL_PCM__)

#ifndef __MOCKING_MODE__
	pcm_ = PCM::getInstance();

	PCM::ErrorCode status = pcm_->program();

	if (status != PCM::Success) {

		//std::cerr << "Intel PCM couldn't start: " << PCM::getErrorCodeString(status) << std::endl;
		//return 1;

		//pcm_->checkStatus(status);

		// try catch, release
		std::cerr << "Intel PCM couldn't start: " << status << std::endl;
		pcm_->cleanup();
		return;
	}

	printf("PCM instance %xllu \n", pcm_);

	// overwrite
	nCores_ = pcm_->getNumCores();
#else
	pcm_ = nullptr;
	nCores_ = 16;
#endif

}

// Intel PCM
void PerformanceMonitorIntel::init() {

	PerformanceMonitorCore::init();  	
	assert(vectorUtilization_.size() == nCores_);

#ifndef __MOCKING_MODE__
	// Take first snapshot
	pcmSystemStateBefore_.push_back(getSystemCounterState());
	for (int i = 0; i < nCores_; ++i)
		pcmCoreStateBefore_.push_back(getCoreCounterState(i));

	// Sleep for a 1 ms (measurement interval)
	Sleep(10);

	// Take second snapshot
	pcmSystemStateAfter_.push_back(getSystemCounterState());
	for (int i = 0; i < nCores_; ++i)
		pcmCoreStateAfter_.push_back(getCoreCounterState(i));
#endif
}


void PerformanceMonitorIntel::release() {
#ifdef WIN32
#ifndef __MOCKING_MODE__
	pcm_->cleanup();
#endif
#endif
}




void PerformanceMonitorIntel::pushPerformanceCounter() {  // int time

	const bool debug_ = false;

	// Move second snapshots to first
//#ifndef __MOCKING_MODE__
	pcmSystemStateBefore_.swap(pcmSystemStateAfter_);
	pcmCoreStateBefore_.swap(pcmCoreStateAfter_);
//#else
//#endif


	// Take second snapshot
#ifndef __MOCKING_MODE__
#ifndef  __REDESIGN__
	pcmSystemStateAfter_.clear();
	pcmSystemStateAfter_.push_back(getSystemCounterState());
	pcmCoreStateAfter_.clear();
	for (int i = 0; i < nCores_; ++i)
		pcmCoreStateAfter_.push_back(getCoreCounterState(i));
#else
	pcmSystemStateAfter_[0] = pcm_->getSystemCounterState();   // alt.: aggreage, .. 
	for (int i = 0; i < nCores_; ++i) {		
		/*  CoreCounterState getCoreCounterState(uint32 core);
			Reads the counter state of a (logical) core
			Be aware that during the measurement other threads may be scheduled on the same core by the operating system (this is called context-switching). The performance events caused by these threads will be counted as well.
		*/
		pcmCoreStateAfter_[i] = pcm_->getCoreCounterState(i);
	}
#endif
#else
	pcmSystemStateAfter_.clear();
	pcmSystemStateAfter_.push_back(nullptr);
	pcmCoreStateAfter_.clear();
	for (int i = 0; i < nCores_; ++i)
		pcmCoreStateAfter_.push_back(nullptr);
#endif

#ifndef __MOCKING_MODE__
	double s0engy = getConsumedJoules(pcmSystemStateBefore_.front(), pcmSystemStateAfter_.front()); // first socket
#else
	double s0engy = 1.0;
#endif

	debug_?printf("Core    UTIL    IPC    FREQ(GHz)  ENGY(J) %1.2f\n", s0engy):0;

	double utilTotal = .0;
	for (int i = 0; i < nCores_; ++i) {
#ifndef __MOCKING_MODE__
		double util = getCoreCStateResidency(0, pcmCoreStateBefore_[i], pcmCoreStateAfter_[i]); // UTIL  : utlization (same as core C0 state active state residency, the value is in 0..1)				
		utilTotal += util;
		double ipc = getIPC(pcmCoreStateBefore_[i], pcmCoreStateAfter_[i]); // instructions per CPU cycle
		double freq = getAverageFrequency(pcmCoreStateBefore_[i], pcmCoreStateAfter_[i]) / 1000000000.; // GHz
#else
		double util = 0.5;
		utilTotal += util;
		double ipc = 0.5;
		double freq = 0.f;
#endif

		vectorUtilization_[i].push_back((float)util);
		vectorInstructions_[i].push_back((float)ipc);
		vectorFrequency_[i].push_back((float)freq);

		//std::cout << "Core " << i << ": "
	}

	
	for (int i = 0; i < nCores_; ++i) {
		double util = vectorUtilization_[i].back();
		double engy = s0engy * util / utilTotal;
		vectorEnergy_[i].push_back((float)engy);
	}

	if (debug_)
		for (int i = 0; i < nCores_; ++i)
			printf("%2d      %1.2f    %1.2f   %1.3f      %1.2f\n", i, 
				vectorUtilization_[i].back(), vectorInstructions_[i].back(), 
				vectorFrequency_[i].back(), vectorEnergy_[i].back());
}

