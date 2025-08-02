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

#include <performance_monitor.h>

#include <performance_monitor_core.h>	// PerformanceMonitor private implementation

#include <user_errors.h>		// fancy user error messages

#include <sstream>				// std::stringstream
#include <algorithm>			// std::transform


// we aren't using namespace std so pay attention!
PerformanceMonitor::PerformanceMonitor(PerformanceMonitorCore* performanceMonitorCorePtr){
	// make sure the pointer is NULL
	performanceMonitorCorePtr_ = performanceMonitorCorePtr;
}

PerformanceMonitor::~PerformanceMonitor() {
	delete performanceMonitorCorePtr_;
}

void PerformanceMonitor::clear(){
	std::string funcName = "clear()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	performanceMonitorCorePtr_->clear();
}

bool PerformanceMonitor::isRecording(){
	return performanceMonitorCorePtr_->isRecording();
}

void PerformanceMonitor::startRecording() {
	std::string funcName = "startRecording()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	performanceMonitorCorePtr_->startRecording();
}

void PerformanceMonitor::stopRecording(){
	std::string funcName = "stopRecording()";
	UserErrors::assertTrue(isRecording(), UserErrors::MUST_BE_ON, funcName, "Recording");

	performanceMonitorCorePtr_->stopRecording();
}

void PerformanceMonitor::setLogFile(const std::string& fileName) {
	std::string funcName = "setLogFile";

	FILE* fid;
	std::string fileNameLower = fileName;
	std::transform(fileNameLower.begin(), fileNameLower.end(), fileNameLower.begin(), ::tolower);

	if (fileNameLower == "null") {
		// user does not want a binary created
		fid = NULL;
	} else {
		fid = fopen(fileName.c_str(),"wb");
		//printf("%s\n", fileName.c_str());
		if (fid==NULL) {
			// default case: print error and exit
			std::string fileError = " Double-check file permissions and make sure directory exists.";
			UserErrors::assertTrue(false, UserErrors::FILE_CANNOT_OPEN, funcName, fileName, fileError);
		}
	}

	// tell new file id to core object
	performanceMonitorCorePtr_->setPerformanceFileId(fid);
}

void PerformanceMonitor::print(bool meanOnly) {
	std::string funcName = "print()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	performanceMonitorCorePtr_->print(meanOnly);
}




bool PerformanceMonitor::getPersistentData() {
	return performanceMonitorCorePtr_->getPersistentData();
}

void PerformanceMonitor::setPersistentData(bool persistentData) {
	performanceMonitorCorePtr_->setPersistentData(persistentData);
}


int PerformanceMonitor::getLastUpdated() {
	std::string funcName = "getLastUpdated()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	return performanceMonitorCorePtr_->getLastUpdated();
}


const int PerformanceMonitor::getCores() {	
	return performanceMonitorCorePtr_->getCores();
}


void PerformanceMonitor::setSampleRate(int ms) {
	performanceMonitorCorePtr_->setSampleRate(ms);
}


const std::vector<std::vector<float>> &PerformanceMonitor::getUtilization(){
	std::string funcName = "getUtilization()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	return performanceMonitorCorePtr_->getUtilization();
}

const std::vector<std::vector<float>> &PerformanceMonitor::getInstructions() {
	std::string funcName = "getInstructions()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	return performanceMonitorCorePtr_->getInstructions();
}

const std::vector<std::vector<float>> &PerformanceMonitor::getFrequency() {
	std::string funcName = "getFrequency()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	return performanceMonitorCorePtr_->getFrequency();
}

const std::vector<std::vector<float>> &PerformanceMonitor::getEnergy() {
	std::string funcName = "getEnergy()";
	UserErrors::assertTrue(!isRecording(), UserErrors::CANNOT_BE_ON, funcName, "Recording");

	return performanceMonitorCorePtr_->getEnergy();
}

