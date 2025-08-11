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

#include <performance_monitor_ms.h>

#include <snn.h>				// CARLsim private implementation
#include <snn_definitions.h>	// KERNEL_ERROR, KERNEL_INFO, ...

#include <algorithm>			// std::sort

//#if defined(WIN32) && (__INTEL_PCM__)
//using namespace pcm;


PerformanceMonitorMs::PerformanceMonitorMs(SNN* snn, int monitorId, int sampleRate): 
	PerformanceMonitorCore(snn, monitorId, sampleRate) 
{

	pdhQuery_ = INVALID_HANDLE_VALUE;

	// defer all unsafe operations to init function
	pre_init();
	init();
	post_init();
}

PerformanceMonitorMs::~PerformanceMonitorMs() {
	release(); 
}

// MS PDH
void PerformanceMonitorMs::pre_init() {
#ifdef WIN32

	PerformanceMonitorCore::pre_init();

	PDH_STATUS status = PdhOpenQuery(NULL, 0, &pdhQuery_);
	if (status != ERROR_SUCCESS) {
		pdhQuery_ = INVALID_HANDLE_VALUE;
		// handle error, KERNEL_ERROR
		return;
	}

	// overwrite
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	nCores_ = sysinfo.dwNumberOfProcessors;

#endif
}

// MS PDH
void PerformanceMonitorMs::init() {
#ifdef WIN32

	PerformanceMonitorCore::init();
	assert(vectorUtilization_.size() == nCores_);

	PDH_STATUS status;

	pdhCounter_.resize(nCores_); // new vector 

	for (int coreIndex = 0; coreIndex < nCores_; coreIndex++) {

		//std::wstring counterPath = L"\\Processor(" + std::to_wstring(coreIndex) + L")\\% Processor Time"; // for Unicode 
		std::string counterPath = "\\Processor(" + std::to_string(coreIndex) + ")\\% Processor Time";
					
		//PDH_HCOUNTER& counter = pdhCounter_[coreIndex];

		//status = PdhAddCounter(pdhQuery_, counterPath.c_str(), 0, &counter);
		status = PdhAddCounter(pdhQuery_, counterPath.c_str(), 0, &pdhCounter_[coreIndex]);
		if (status == ERROR_SUCCESS) {
//			pdhCounter_[coreIndex] = counter;
		}
		else 
		{
			PdhCloseQuery(pdhQuery_);
			pdhQuery_ = INVALID_HANDLE_VALUE;
			// handle error, KERNEL_ERROR
			return;
		}
	}

	if (PdhCollectQueryData(pdhQuery_) != ERROR_SUCCESS) {
		pdhQuery_ = INVALID_HANDLE_VALUE;
		return;
	}

	//Sleep(1000); // Following MS recommendation some time to init new counters

#endif
}

void PerformanceMonitorMs::release() {
#ifdef WIN32
	if(pdhQuery_ != INVALID_HANDLE_VALUE)
		PdhCloseQuery(pdhQuery_);
#endif
}


/*

HANDLE hPdhLibrary = NULL;
LPWSTR pMessage = NULL;
DWORD dwErrorCode = PDH_PLA_ERROR_ALREADY_EXISTS;



if (!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE |
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	hPdhLibrary,
	dwErrorCode,
	0,
	(LPWSTR)&pMessage,
	0,
	NULL))
{
	wprintf(L"Format message failed with 0x%x\n", GetLastError());
	return;
}

wprintf(L"Formatted message: %ls\n", pMessage);
LocalFree(pMessage);
*/


void PerformanceMonitorMs::pushPerformanceCounter() {  // int time

	const bool debug_ = false;

	PDH_STATUS status;

	if (pdhQuery_ == INVALID_HANDLE_VALUE)
		return; 

	status = PdhCollectQueryData(pdhQuery_);
	if(status != ERROR_SUCCESS) {
		pdhQuery_ = INVALID_HANDLE_VALUE;
		return;
	}
	
	//for (int i = 0; i < pdhCounter_.size(); i++) {   // consider C++11 iterator   // nCores = 4
	//for (int i = 0; i < 12; i++) {   // consider C++11 iterator   // nCores = 4
	for (int i = 0; i < nCores_; i++) {   // consider C++11 iterator   
		PDH_HCOUNTER counter = pdhCounter_[i];
		PDH_FMT_COUNTERVALUE value;
		double util = -1.0;
		status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);
	    //unsigned int code = status & 0xFFFFFFFF;
		//#define PDH_INVALID_DATA                 0xC0000BC6L

		if (status == ERROR_SUCCESS) {
			util = value.doubleValue;
		} else {			
			//e.g. status == PDH_INVALID_DATA) {
			util = -1.0;
		}
			//util = (status & 0xFFFFFFF) * -1;
			//PdhCloseQuery(pdhQuery_);
			//pdhQuery_ = INVALID_HANDLE_VALUE;
			//return;
		
		//vectorPdhCoreUtilization_[i].push_back((float)util);
		vectorUtilization_[i].push_back((float)util);   // DONE REFACT   vectorCoreUtilization_
		if(debug_) printf("%7.1f%%", util);
	}

	if (debug_) printf("\n");


}







