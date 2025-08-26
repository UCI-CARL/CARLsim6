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
#include <performance_monitor_intel.h>
#include <performance_monitor_ms.h>

#include <snn.h>				// CARLsim private implementation
#include <snn_definitions.h>	// KERNEL_ERROR, KERNEL_INFO, ...
//#include <error_code.h>

#include <algorithm>			// std::sort


PerformanceMonitorCore* PerformanceMonitorCore::create(SNN* snn, int monitorId, PerformanceMonitorBackend backend, int sampleRate) {

	// Enum is defined in see carlsim_definitions.h   
	switch (backend) {
		//case PMB_MS: return new PerformanceMonitorMs(snn, monitorId, sampleRate);
		case PMB_INTEL: return new PerformanceMonitorIntel(snn, monitorId, sampleRate);
		// ARM, AMD, LNX ... 
		default: 
			throw "Unsupported PerformanceMonitor Backend.";
			//KERNEL_ERROR("Unsupported PerformanceMonitor Backend.");
			//exitSimulation(KERNEL_ERROR_PMB_UNKNOWN);
	}

}



PerformanceMonitorCore::PerformanceMonitorCore(SNN* snn, int monitorId, int sampleRate): 
	snn_(snn), monitorId_(monitorId), sampleRate_(sampleRate) 
{
	performanceFileId_ = NULL;
	recordSet_ = false;
	performanceMonLastUpdated_ = 0;

	persistentData_ = false;
	userHasBeenWarned_ = false;
	needToWriteFileHeader_ = true;
	performanceFileSignature_ = 206661982;
	performanceFileVersion_ = 1.0f;

	// defaults
	nCores_ = 0;

	// defer all unsafe operations to init function in derived class
}

PerformanceMonitorCore::~PerformanceMonitorCore() {


}

void PerformanceMonitorCore::init() {


	assert(nCores_ > 0);  // parent needs to be called after 

	vectorUtilization_.resize(nCores_);
	vectorInstructions_.resize(nCores_);
	vectorFrequency_.resize(nCores_);
	vectorEnergy_.resize(nCores_);

	clear();

	// use KERNEL_{ERROR|WARNING|etc} typesetting (const FILE*)
	fpInf_ = snn_->getLogFpInf();
	fpErr_ = snn_->getLogFpErr();
	fpDeb_ = snn_->getLogFpDeb();
	fpLog_ = snn_->getLogFpLog();


}


void PerformanceMonitorCore::clear() {

	// needs to be called before derived 

	assert(!isRecording());
	recordSet_ = false;
    userHasBeenWarned_ = false;
	startTime_ = -1;
	startTimeLast_ = -1;
	stopTime_ = -1;
	accumTime_ = 0;
	totalTime_ = -1;

	for (int i = 0; i < nCores_; i++) {
		vectorUtilization_[i].clear();
		vectorInstructions_[i].clear();
		vectorFrequency_[i].clear();
		vectorEnergy_[i].clear();
	}

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
	Grid3D grid(nCores_, 1, 1);  // REFACT Y slices,  Z counter
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


long int PerformanceMonitorCore::getBufferSize() {
	long int bufferSize = 0; // in bytes
	for (int coreIndex = 0; coreIndex < vectorUtilization_.size(); coreIndex++) {  //nCores_
		bufferSize += vectorUtilization_[coreIndex].size() *
			(sizeof(int) + sizeof(int)  // t, core, 
				+ sizeof(float) // pcm_util,
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


const std::vector<std::vector<float>> & PerformanceMonitorCore::getUtilization(){
	return vectorUtilization_;
}

const std::vector<std::vector<float>> &PerformanceMonitorCore::getInstructions() {
	return vectorInstructions_;
}

const std::vector<std::vector<float>> &PerformanceMonitorCore::getFrequency() {
	return vectorFrequency_;
}

const std::vector<std::vector<float>> &PerformanceMonitorCore::getEnergy() {
	return vectorEnergy_;
}


//
void PerformanceMonitorCore::print(bool meanOnly) {
	assert(!isRecording());

}