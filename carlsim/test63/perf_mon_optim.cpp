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

#include <cmath>
#include <vector>

const int N_offset = 0;
const int N_cores = 12;

//const int N_offset = 4;  // does not really make sense as it shall monitor the processor as a given
//const int N_cores = 4;


/*unsigned long long N_calc = 0;
//unsigned long long N_calc = 1000;
//unsigned long long N_calc = 10000000;
int N_iter = 100;
//unsigned long long N_calc = 100000000;
//int N_iter = 100;

int N_sleep = 1000; 

unsigned long long result, result2, result3;

void do_calculations(int n) {
	for (int i = 0; i < n; i++) {
		result += 1;
		result2 += i;
		result3 += result3 * 1.111 / 3.14 * (std::sqrt(result3) * (result2 * (result2 * 0.001 + 20)));
	}
}


#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <iostream>
#include <string>

#pragma comment(lib, "pdh.lib")

PDH_STATUS PdhStatus1, PdhStatus2; 
PDH_HQUERY PdhQuery;
PDH_HCOUNTER PdhCounter[N_cores];

std::vector<char[255]> PdhCounterPath(N_cores); 


// Returns CPU utilization percentage for the given core index (0-based)
void init_counter() {

	PDH_STATUS status = PdhOpenQuery(NULL, 0, &PdhQuery);

	if(status != ERROR_SUCCESS) {
		printf("error: %d\n", status);
		PdhCloseQuery(PdhQuery);
		return;
	}

	//for (int coreIndex = 0; coreIndex < N_cores; coreIndex++) {
	//	//std::wstring counterPath = L"\\Processor(" + std::to_wstring(coreIndex) + L")\\% Processor Time";
	//	sprintf(PdhCounterPath[coreIndex], "\\Processor(%d)\\%% Processor Time", coreIndex);
	//}
	for (int coreIndex = 0; coreIndex < N_cores; coreIndex++) {
		//std::wstring counterPath = L"\\Processor(" + std::to_wstring(coreIndex) + L")\\% Processor Time";
		std::string counterPath = "\\Processor(" + std::to_string(coreIndex+N_offset) + ")\\% Processor Time";
		//if (PdhAddEnglishCounter(PdhQuery, PdhCounterPath[coreIndex], 0, &PdhCounter[coreIndex]) != ERROR_SUCCESS) {
		if (PdhAddEnglishCounter(PdhQuery, counterPath.c_str(), 0, &PdhCounter[coreIndex]) != ERROR_SUCCESS) {
			PdhCloseQuery(PdhQuery);
			return;
		}
	}

	PdhStatus1 = PdhCollectQueryData(PdhQuery);

	Sleep(100); // Following MS recommendation some time to init new counters

}

void release_counter() {
	PdhCloseQuery(PdhQuery);
}

// https://learn.microsoft.com/en-us/windows/win32/api/pdh/nf-pdh-pdhaddcountera

// Returns CPU utilization percentage for the given core index (0-based)
void get_cpu_core_utilization() {
	//PDH_HQUERY query;
	//PDH_HCOUNTER counter;
	//std::wstring counterPath = L"\\Processor(" + std::to_wstring(coreIndex) + L")\\% Processor Time";
	//std::string counterPath = "\\Processor(" + std::to_string(coreIndex) + ")\\% Processor Time";

	//std::string counterPath = "\\Processor(" + std::to_string(coreIndex) + ")\\% Processor Time";

	//if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
	//	return -1.0;
	//if (PdhAddEnglishCounter(query, counterPath.c_str(), 0, &counter) != ERROR_SUCCESS) {
	//	PdhCloseQuery(query);
	//	return -1.0;
	//}

	//if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
	//	return -1.0;
	//if (PdhAddEnglishCounter(query, counterPath[coreIndex], 0, &counter[coreIndex]) != ERROR_SUCCESS) {
	//	PdhCloseQuery(query);
	//	return -1.0;
	//}

	//if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
	//	return -1.0;
	//if (PdhAddEnglishCounter(query, counterPath[coreIndex], 0, &counter) != ERROR_SUCCESS) {
	//	PdhCloseQuery(query);
	//	return -1.0;
	//}


	//PdhStatus1 = PdhCollectQueryData(PdhQuery);

	//Sleep(1);
	//do_calculations(N_calc);
	//Sleep(1);
	//Sleep(100); // Wait 100 ms to get a valid reading

	PdhStatus2 = PdhCollectQueryData(PdhQuery);		

	for (int coreIndex = 0; coreIndex < N_cores; coreIndex++) {

		double util = -1.0;
		PDH_FMT_COUNTERVALUE value;
		if (PdhGetFormattedCounterValue(PdhCounter[coreIndex], PDH_FMT_DOUBLE, NULL, &value) == ERROR_SUCCESS) {
			util = value.doubleValue;
		}
//			PdhCloseQuery(PdhQuery);
//			return -1.0;
		
		//if (util >= 0.0)
			printf("%7.1f%%", value.doubleValue);
		//else
		//	printf("        ");

	}

	printf("\n");


}


TEST(PerfMon, pdh) {

	for (int core = 0; core < N_cores; core++) {
		printf("  core%02d", core + N_offset);
	}
	printf("\n");

	init_counter();
	
	for (int i = 0; i < N_iter; i++) {
		//do_calculations(N_calc);
		get_cpu_core_utilization();
		Sleep(N_sleep);
	}
		

	release_counter();

	EXPECT_EQ(result, N_iter * N_calc);

}

*/