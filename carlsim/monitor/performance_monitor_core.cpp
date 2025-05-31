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

#include <performance_monitor_core.h>

#include <snn.h>				// CARLsim private implementation
#include <snn_definitions.h>	// KERNEL_ERROR, KERNEL_INFO, ...

#include <algorithm>			// std::sort




#if defined(WIN32) && defined(__INTEL_CPU__)
#include <windows.h>
#include "EnergyLib.h" // Make sure this is in your include path  // Obsolete with PCM
#endif


PerformanceMonitorCore::PerformanceMonitorCore(SNN* snn, int monitorId) {
	snn_ = snn;

	monitorId_ = monitorId;
	performanceFileId_ = NULL;
	recordSet_ = false;
	performanceMonLastUpdated_ = 0;

	persistentData_ = false;
    userHasBeenWarned_ = false;
	needToWriteFileHeader_ = true;
    performanceFileSignature_ = 206661982;
	performanceFileVersion_ = 1.0f;

	// defer all unsafe operations to init function
	init();
}

PerformanceMonitorCore::~PerformanceMonitorCore() {

}

void PerformanceMonitorCore::init() {

	nCores_ = 12;  // TODO 
	//nSockets_= 1;

	initMsPdh();

	//initIntelPcm();

	//initNvidaNvml()

	// always manage (create, initialize, release) all performance counter
	// first dim is the cores, second dim their values over time
	vectorPdhCoreUtilization_.resize(nCores_);  

	vectorPcmCoreUtilization_.resize(nCores_);
	vectorPcmCoreInstructions_.resize(nCores_);
	vectorPcmCoreFrequency_.resize(nCores_);
	vectorPcmCoreTemperatur_.resize(nCores_);
	vectorPcmCoreEnergy_.resize(nCores_);

	clear();

	// use KERNEL_{ERROR|WARNING|etc} typesetting (const FILE*)
	fpInf_ = snn_->getLogFpInf();
	fpErr_ = snn_->getLogFpErr();
	fpDeb_ = snn_->getLogFpDeb();
	fpLog_ = snn_->getLogFpLog();
}


// MS PDH
void PerformanceMonitorCore::initMsPdh() {
#ifdef WIN32
	
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &pdhQuery_);
	if (status != ERROR_SUCCESS) {
		pdhQuery_ = INVALID_HANDLE_VALUE;
		// handle error, KERNEL_ERROR
		return;
	}

//	pdhCounter_.resize(nCores_);

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

void PerformanceMonitorCore::releaseMsPdh() {
#ifdef WIN32
	if(pdhQuery_ != INVALID_HANDLE_VALUE)
		PdhCloseQuery(pdhQuery_);
#endif
}

// Intel PCM
void PerformanceMonitorCore::initIntelPcm() {

	//
	//#if defined(WIN32) && (__INTEL_CPU__)
	//
	//if (!IntelEnergyLibInitialize()) {
	//	std::cerr << "Failed to initialize Intel Power Gadget EnergyLib." << std::endl;
	//	return 1;
	//}
	//int nDomains = 0;
	//GetNumNodes(&nDomains); // Number of physical CPU packages
	//int nCores = 0;
	//GetNumCores(&nCores);
	//int nMsrs = 0;
	//GetNumMsrs(&nMsrs);
	//

}


void PerformanceMonitorCore::clear() {
	assert(!isRecording());
	recordSet_ = false;
    userHasBeenWarned_ = false;
	startTime_ = -1;
	startTimeLast_ = -1;
	stopTime_ = -1;
	accumTime_ = 0;
	totalTime_ = -1;

	for (int i = 0; i < nCores_; i++) {
		vectorPdhCoreUtilization_[i].clear();
		vectorPcmCoreInstructions_[i].clear();
		vectorPcmCoreFrequency_[i].clear();
		vectorPcmCoreTemperatur_[i].clear();
		vectorPcmCoreEnergy_[i].clear();
	}

}

/*
	void SpikeMonitorCore::pushAER(int time, int neurId) {
		assert(isRecording());
		assert(getMode() == AER);

		spkVector_[neurId].push_back(time);
}*/

void PerformanceMonitorCore::armMsPdh() {  // int time

	//if (pdhQuery_ == INVALID_HANDLE_VALUE)
	//	return;

	//if(PdhCollectQueryData(pdhQuery_) != ERROR_SUCCESS)
	//	pdhQuery_ = INVALID_HANDLE_VALUE;

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


void PerformanceMonitorCore::pushMsPdh() {  // int time

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
	for (int i = 0; i < 12; i++) {   // consider C++11 iterator   // nCores = 4
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
		
		vectorPdhCoreUtilization_[i].push_back((float)util);
		if(debug_) printf("%7.1f%%", util);
	}

	if (debug_) printf("\n");


}







void PerformanceMonitorCore::pushIntelPcm() {  // int time


	//(int neurId, float AMPA, float NMDA, float GABAa, float GABAb
//
//void PerformanceMonitorCore::pushCpu() {
//	assert(isRecording());
////
////#if defined(__WIN32__) && (__INTEL_CPU__)	
////
////	// Take reading
////	ReadSample();
////
////
////	for (int i = 0; i < nDomains; ++i) {
////		double power = 0.0;
////		GetPowerData(i, 0, 0, &power); // 0: Package, 1: IA, 2: DRAM, 3: GPU, etc.
////		std::cout << "Package " << i << " Power: " << power << " W" << std::endl;
////
////		double utilization = 0.0;
////		GetIAUtilization(i, &utilization); // IA utilization (core active ratio, 0-1)
////		std::cout << "Package " << i << " IA Utilization: " << utilization * 100.0 << " %" << std::endl;
////
////
////	}
////	// For per-core utilization, you can use GetIAUtilization for each package.
////	// Intel Power Gadget does not provide per-core power, only per-package.
////
////C:\Test\github\pcm\examples
////	printf("[c_example] Calling PCM start()\n");
////	PCM.pcm_c_start();
////	for (i = 0; i < 10000; i++)
////		c[i % 100] = 4 * a[i % 100] + b[i % 100];
////	for (i = 0; i < 100; i++)
////		total += c[i];
////	PCM.pcm_c_stop();
////
////	printf("[c_example] PCM measurement stopped, compute result %u\n", total);
////
////	lcore_id = pcm_getcpu();
////	printf("C:%llu I:%llu, IPC:%3.2f\n",
////		(unsigned long long)PCM.pcm_c_get_cycles(lcore_id),
////		(unsigned long long)PCM.pcm_c_get_instr(lcore_id),
////		(double)PCM.pcm_c_get_instr(lcore_id) / PCM.pcm_c_get_cycles(lcore_id));
////	printf("CPU%d E0: %llu, E1: %llu, E2: %llu, E3: %llu\n",
////		lcore_id,
////		(unsigned long long)PCM.pcm_c_get_core_event(lcore_id, 0),
////
////
////	if (neurId >= MAX_COBA_MON_GRP_SIZE)
////		return; // ignore values as the are empty anyway (see buffer transfer)
////
////	//printf("pushPerformance %d %f %f %f %f\n", neurId, AMPA, NMDA, GABAa, GABAb);
////
////	vectorAMPA_[neurId].push_back(AMPA);
////    vectorNMDA_[neurId].push_back(NMDA);
////    vectorGABAa_[neurId].push_back(GABAa);
////	vectorGABAb_[neurId].push_back(GABAb);
////
////	// update mean 
////	const int n = vectorAMPA_[neurId].size();
////	if (vectorAMPA_[nNeurons_].size() < n) {
////		// set first element
////		vectorAMPA_[nNeurons_].push_back(AMPA / nNeurons_);
////		vectorNMDA_[nNeurons_].push_back(NMDA / nNeurons_);
////		vectorGABAa_[nNeurons_].push_back(GABAa / nNeurons_);
////		vectorGABAb_[nNeurons_].push_back(GABAb / nNeurons_);
////	}
////	else {
////		// mean record was appended by other neuron
////		vectorAMPA_[nNeurons_][n-1] += AMPA / nNeurons_;
////		vectorNMDA_[nNeurons_][n-1] += NMDA / nNeurons_;
////		vectorGABAa_[nNeurons_][n-1] += GABAa / nNeurons_;
////		vectorGABAb_[nNeurons_][n-1] += GABAb / nNeurons_;
////	}
//}


}






void PerformanceMonitorCore::startRecording() {
	assert(!isRecording());

	if (!persistentData_) {
		// if persistent mode is off (default behavior), automatically call clear() here
		clear();
	}

	// call updatePerformanceMonitor to make sure neuron state file and neuron state vector are up-to-date
	// Caution: must be called before recordSet_ is set to true!
	snn_->updatePerformanceMonitor();

	recordSet_ = true;
	long int currentTime = snn_->getSimTimeSec()*1000+snn_->getSimTimeMs();

	if (persistentData_) {
		// persistent mode on: accumulate all times
		// change start time only if this is the first time running it
		startTime_ = (startTime_<0) ? currentTime : startTime_;
		startTimeLast_ = currentTime;
		accumTime_ = (totalTime_>0) ? totalTime_ : 0;
	}
	else {
		// persistent mode off: we only care about the last probe
		startTime_ = currentTime;
		startTimeLast_ = currentTime;
		accumTime_ = 0;
	}
}

void PerformanceMonitorCore::stopRecording() {

	assert(isRecording());
	assert(startTime_>-1 && startTimeLast_>-1 && accumTime_>-1);

	// call updatePerformanceMonitor to make sure neuron state file and neuron state vector are up-to-date
	// Caution: must be called before recordSet_ is set to false!
	snn_->updatePerformanceMonitor();

	recordSet_ = false;
    userHasBeenWarned_ = false;
	stopTime_ = snn_->getSimTimeSec()*1000+snn_->getSimTimeMs();

	// total time is the amount of time of the last probe plus all accumulated time from previous probes
	totalTime_ = stopTime_-startTimeLast_ + accumTime_;
	assert(totalTime_>=0);
}

// returns the total accumulated time.
long int PerformanceMonitorCore::getAccumTime(){
    return accumTime_;
}

void PerformanceMonitorCore::setPerformanceFileId(FILE* neuronFileId) {
	assert(!isRecording());

	// close previous file pointer if exists
	if (performanceFileId_!=NULL) {
		fclose(performanceFileId_);
		performanceFileId_ = NULL;
	}

	// set it to new file id
	performanceFileId_=neuronFileId;

	if (performanceFileId_==NULL)
		needToWriteFileHeader_ = false;
	else {
		// file pointer has changed, so we need to write header (again)
		needToWriteFileHeader_ = true;
		writePerformanceFileHeader();
	}
}

// write the header section of the neuron state file
void PerformanceMonitorCore::writePerformanceFileHeader() {
	if (!needToWriteFileHeader_)
		return;

	// write file signature
	if (!fwrite(&performanceFileSignature_,sizeof(int),1,performanceFileId_))
		KERNEL_ERROR("PerformanceMonitorCore: writePerformanceFileHeader has fwrite error");

	// write version number
	if (!fwrite(&performanceFileVersion_,sizeof(float),1,performanceFileId_))
		KERNEL_ERROR("PerformanceMonitorCore: writePerformanceFileHeader has fwrite error");

	// write grid dimensions
	Grid3D grid(nCores_, 1, 1);
	int tmpInt = grid.numX;
	if (!fwrite(&tmpInt, sizeof(int), 1, performanceFileId_))
		KERNEL_ERROR("PerformanceMonitorCore: writePerformanceFileHeader has fwrite error");
	tmpInt = grid.numY;
	if (!fwrite(&tmpInt, sizeof(int), 1, performanceFileId_))
		KERNEL_ERROR("PerformanceMonitorCore: writePerformanceFileHeader has fwrite error");
	tmpInt = grid.numZ;
	if (!fwrite(&tmpInt, sizeof(int), 1, performanceFileId_))
		KERNEL_ERROR("PerformanceMonitorCore: writePerformanceFileHeader has fwrite error");

	// write MAX_PERF_MON_CTR_SIZE
	int max_performance_mon_counter_size = MAX_PERF_MON_CTR_SIZE;
	if (!fwrite(&max_performance_mon_counter_size,sizeof(int),1,performanceFileId_))
		KERNEL_ERROR("PerformanceMonitorCore: writePerformanceFileHeader has fwrite error");

	needToWriteFileHeader_ = false;

}

long int PerformanceMonitorCore::getBufferSize(){
    long int bufferSize=0; // in bytes
    for(int coreIndex=0; coreIndex < vectorPdhCoreUtilization_.size(); coreIndex++) {  //nCores_
        bufferSize += vectorPdhCoreUtilization_[coreIndex].size() * 
			(sizeof(int) + sizeof(int)  // t, core, 
				+ sizeof(float) // pdh_util,
				// ...  -> MAX_PERF_MON_CTR_SIZE;
			);   
    }
	return bufferSize;
}

// check if the state vector is getting large. If it is, return true once until
// stopRecording is called.
bool PerformanceMonitorCore::isBufferBig(){
    if(userHasBeenWarned_)
        return false;
    else {
        //check if buffer is too big
        if(this->getBufferSize()>MAX_PERF_MON_BUFFER_SIZE){
            userHasBeenWarned_=true;
            return true;
        }
        else {
            return false;
        }
    }
}

// TODO const &  to avoid cloning/copy

const std::vector<std::vector<float> > & PerformanceMonitorCore::getPdhCoreUtilization() {
	//assert(!isRecording());  // not relevant for perf mon
	return vectorPdhCoreUtilization_;
}


// NYI

std::vector<std::vector<float> > PerformanceMonitorCore::getPcmCoreUtilization(){
	assert(!isRecording());
	return vectorPcmCoreUtilization_;
}

std::vector<std::vector<float> > PerformanceMonitorCore::getPcmCoreInstructions() {
	assert(!isRecording());
	return vectorPcmCoreInstructions_;
}

std::vector<std::vector<float> > PerformanceMonitorCore::getPcmCoreFrequency() {
	assert(!isRecording());
	return vectorPcmCoreFrequency_;
}

std::vector<std::vector<float> > PerformanceMonitorCore::getPcmCoreTemperatur() {
	assert(!isRecording());
	return vectorPcmCoreTemperatur_;
}

std::vector<std::vector<float> > PerformanceMonitorCore::getPcmCoreEnergy() {
	assert(!isRecording());
	return vectorPcmCoreEnergy_;
}


// Rework, 1000 ms, buffer of AMPA, NMDA, GABAa, GABAb, by N Neurons 
//
void PerformanceMonitorCore::print(bool meanOnly) {
	assert(!isRecording());

/* see ...
		
		
		vectorPdhCoreUtilization_[i].push_back((float)util);
		if(debug_) printf("%7.1f%%", util);
	}

	if (debug_) printf("\n");

*/

//	// how many spike times to display per row
//	int dispAmpaPerRow = 7;
//
//	// spike times only available in AER mode
//	KERNEL_INFO("| Neur ID | AMPA");
//	KERNEL_INFO("|- - - - -|- - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -")
//
//	for (int i=meanOnly?nNeurons_:0; i<=nNeurons_; i++) {  // with mean 
//		char buffer[100];
//		if (i < nNeurons_) {
//#if defined(WIN32) || defined(WIN64)
//			_snprintf(buffer, 100, "| %7d | ", i);
//#else
//			snprintf(buffer, 100, "| %7d | ", i);
//#endif
//		}
//		else {
//#if defined(WIN32) || defined(WIN64)
//			_snprintf(buffer, 100, "| %7s | ", "mean");
//#else
//			snprintf(buffer, 100, "| %7s | ", "mean");
//#endif
//		}
//		int nV = vectorAMPA_[i].size();
//		for (int j=0; j<nV; j++) {
//			char volts[10];
//#if defined(WIN32) || defined(WIN64)
//			_snprintf(volts, 10, "%4.4f ", vectorAMPA_[i][j]);
//#else
//			snprintf(volts, 10, "%4.4f ", vectorV_[i][j]);
//#endif
//			strcat(buffer, volts);
//			if (j%dispAmpaPerRow == dispAmpaPerRow-1 && j<nV-1) {
//				KERNEL_INFO("%s",buffer);
//				strcpy(buffer,"|         |");
//			}
//		}
//		KERNEL_INFO("%s",buffer);
//	}

}