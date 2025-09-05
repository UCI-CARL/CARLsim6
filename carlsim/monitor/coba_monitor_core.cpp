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

#include <coba_monitor_core.h>

#include <snn.h>				// CARLsim private implementation
#include <snn_definitions.h>	// KERNEL_ERROR, KERNEL_INFO, ...

#include <algorithm>			// std::sort

CobaMonitorCore::CobaMonitorCore(SNN* snn, int monitorId, int grpId) {
	snn_ = snn;
	grpId_= grpId;
	monitorId_ = monitorId;
	nNeurons_ = -1;
	cobaFileId_ = NULL;
	recordSet_ = false;
	cobaMonLastUpdated_ = 0;

	persistentData_ = false;
    userHasBeenWarned_ = false;
	needToWriteFileHeader_ = true;
    cobaFileSignature_ = 206661980;
	cobaFileVersion_ = 0.1f;

	// defer all unsafe operations to init function
	init();
}

void CobaMonitorCore::init() {
#if defined(WIN32) && defined(__NO_CUDA__)
	nNeurons_ = std::min<int>(MAX_NEURON_MON_GRP_SZIE, snn_->getGroupNumNeurons(grpId_));
#else
	nNeurons_ = std::min(MAX_NEURON_MON_GRP_SZIE, snn_->getGroupNumNeurons(grpId_));
#endif
	assert(nNeurons_>0);

	// so the first dimension is neuron ID
	vectorAMPA_.resize(nNeurons_+1);  // reserve the record last for mean
    vectorNMDA_.resize(nNeurons_+1);
    vectorGABAa_.resize(nNeurons_+1);
	vectorGABAb_.resize(nNeurons_+1);

	clear();

	// use KERNEL_{ERROR|WARNING|etc} typesetting (const FILE*)
	fpInf_ = snn_->getLogFpInf();
	fpErr_ = snn_->getLogFpErr();
	fpDeb_ = snn_->getLogFpDeb();
	fpLog_ = snn_->getLogFpLog();
}

CobaMonitorCore::~CobaMonitorCore() {
	if (cobaFileId_!=NULL) {
		fclose(cobaFileId_);
		cobaFileId_ = NULL;
	}
}

void CobaMonitorCore::clear() {
	assert(!isRecording());
	recordSet_ = false;
    userHasBeenWarned_ = false;
	startTime_ = -1;
	startTimeLast_ = -1;
	stopTime_ = -1;
	accumTime_ = 0;
	totalTime_ = -1;

	for (int i=0; i<=nNeurons_; i++){  // including mean
		vectorAMPA_[i].clear();
        vectorNMDA_[i].clear();
        vectorGABAa_[i].clear();
		vectorGABAb_[i].clear();
	}
}

void CobaMonitorCore::pushCoba(int neurId, float AMPA, float NMDA, float GABAa, float GABAb) {
	assert(isRecording());

	if (neurId >= MAX_COBA_MON_GRP_SIZE)
		return; // ignore values as the are empty anyway (see buffer transfer)

	//printf("pushCoba %d %f %f %f %f\n", neurId, AMPA, NMDA, GABAa, GABAb);

	vectorAMPA_[neurId].push_back(AMPA);
    vectorNMDA_[neurId].push_back(NMDA);
    vectorGABAa_[neurId].push_back(GABAa);
	vectorGABAb_[neurId].push_back(GABAb);

	// update mean 
	const int n = vectorAMPA_[neurId].size();
	if (vectorAMPA_[nNeurons_].size() < n) {
		// set first element
		vectorAMPA_[nNeurons_].push_back(AMPA / nNeurons_);
		vectorNMDA_[nNeurons_].push_back(NMDA / nNeurons_);
		vectorGABAa_[nNeurons_].push_back(GABAa / nNeurons_);
		vectorGABAb_[nNeurons_].push_back(GABAb / nNeurons_);
	}
	else {
		// mean record was appended by other neuron
		vectorAMPA_[nNeurons_][n-1] += AMPA / nNeurons_;
		vectorNMDA_[nNeurons_][n-1] += NMDA / nNeurons_;
		vectorGABAa_[nNeurons_][n-1] += GABAa / nNeurons_;
		vectorGABAb_[nNeurons_][n-1] += GABAb / nNeurons_;
	}
}

void CobaMonitorCore::startRecording() {
	assert(!isRecording());

	if (!persistentData_) {
		// if persistent mode is off (default behavior), automatically call clear() here
		clear();
	}

	// call updateCobaMonitor to make sure neuron state file and neuron state vector are up-to-date
	// Caution: must be called before recordSet_ is set to true!
	snn_->updateCobaMonitor(grpId_);

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

void CobaMonitorCore::stopRecording() {

	assert(isRecording());
	assert(startTime_>-1 && startTimeLast_>-1 && accumTime_>-1);

	// call updateCobaMonitor to make sure neuron state file and neuron state vector are up-to-date
	// Caution: must be called before recordSet_ is set to false!
	snn_->updateCobaMonitor(grpId_);

	recordSet_ = false;
    userHasBeenWarned_ = false;
	stopTime_ = snn_->getSimTimeSec()*1000+snn_->getSimTimeMs();

	// total time is the amount of time of the last probe plus all accumulated time from previous probes
	totalTime_ = stopTime_-startTimeLast_ + accumTime_;
	assert(totalTime_>=0);
}

// returns the total accumulated time.
long int CobaMonitorCore::getAccumTime(){
    return accumTime_;
}

void CobaMonitorCore::setCobaFileId(FILE* neuronFileId) {
	assert(!isRecording());

	// close previous file pointer if exists
	if (cobaFileId_!=NULL) {
		fclose(cobaFileId_);
		cobaFileId_ = NULL;
	}

	// set it to new file id
	cobaFileId_=neuronFileId;

	if (cobaFileId_==NULL)
		needToWriteFileHeader_ = false;
	else {
		// file pointer has changed, so we need to write header (again)
		needToWriteFileHeader_ = true;
		writeCobaFileHeader();
	}
}

// write the header section of the neuron state file
void CobaMonitorCore::writeCobaFileHeader() {
	if (!needToWriteFileHeader_)
		return;

	// write file signature
	if (!fwrite(&cobaFileSignature_,sizeof(int),1,cobaFileId_))
		KERNEL_ERROR("CobaMonitorCore: writeCobaFileHeader has fwrite error");

	// write version number
	if (!fwrite(&cobaFileVersion_,sizeof(float),1,cobaFileId_))
		KERNEL_ERROR("CobaMonitorCore: writeCobaFileHeader has fwrite error");

	// write grid dimensions
	Grid3D grid = snn_->getGroupGrid3D(grpId_);
	int tmpInt = grid.numX;
	if (!fwrite(&tmpInt,sizeof(int),1,cobaFileId_))
		KERNEL_ERROR("CobaMonitorCore: writeCobaFileHeader has fwrite error");

	tmpInt = grid.numY;
	if (!fwrite(&tmpInt,sizeof(int),1,cobaFileId_))
		KERNEL_ERROR("CobaMonitorCore: writeCobaFileHeader has fwrite error");

	tmpInt = grid.numZ;
	if (!fwrite(&tmpInt,sizeof(int),1,cobaFileId_))
		KERNEL_ERROR("CobaMonitorCore: writeCobaFileHeader has fwrite error");

	// write MAX_NEURON_MON_GRP_SZIE
	int max_coba_mon_group_size = MAX_COBA_MON_GRP_SIZE;
	if (!fwrite(&max_coba_mon_group_size,sizeof(int),1,cobaFileId_))
		KERNEL_ERROR("CobaMonitorCore: writeCobaFileHeader has fwrite error");

	needToWriteFileHeader_ = false;
}

long int CobaMonitorCore::getBufferSize(){
    long int bufferSize=0; // in bytes
    for(int i=0; i<vectorAMPA_.size();i++){
        bufferSize+=vectorAMPA_[i].size()*sizeof(int);
    }
    return 4 * bufferSize;
}

// check if the state vector is getting large. If it is, return true once until
// stopRecording is called.
bool CobaMonitorCore::isBufferBig(){
    if(userHasBeenWarned_)
        return false;
    else {
        //check if buffer is too big
        if(this->getBufferSize()>MAX_NEURON_MON_BUFFER_SIZE){
            userHasBeenWarned_=true;
            return true;
        }
        else {
            return false;
        }
    }
}

std::vector<std::vector<float> > CobaMonitorCore::getVectorAMPA(){
	assert(!isRecording());
	return vectorAMPA_;
}

std::vector<std::vector<float> > CobaMonitorCore::getVectorNMDA(){
	assert(!isRecording());
	return vectorNMDA_;
}

std::vector<std::vector<float> > CobaMonitorCore::getVectorGABAa(){
	assert(!isRecording());
	return vectorGABAa_;
}

std::vector<std::vector<float> > CobaMonitorCore::getVectorGABAb() {
	assert(!isRecording());
	return vectorGABAb_;
}

// Rework, 1000 ms, buffer of AMPA, NMDA, GABAa, GABAb, by N Neurons 
//
void CobaMonitorCore::print(bool meanOnly) {
	assert(!isRecording());

	// how many spike times to display per row
	int dispAmpaPerRow = 7;

	// spike times only available in AER mode
	KERNEL_INFO("| Neur ID | AMPA");
	KERNEL_INFO("|- - - - -|- - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -")

	for (int i=meanOnly?nNeurons_:0; i<=nNeurons_; i++) {  // with mean 
		char buffer[100];
		if (i < nNeurons_) {
#if defined(WIN32) || defined(WIN64)
			_snprintf(buffer, 100, "| %7d | ", i);
#else
			snprintf(buffer, 100, "| %7d | ", i);
#endif
		}
		else {
#if defined(WIN32) || defined(WIN64)
			_snprintf(buffer, 100, "| %7s | ", "mean");
#else
			snprintf(buffer, 100, "| %7s | ", "mean");
#endif
		}
		int nV = vectorAMPA_[i].size();
		for (int j=0; j<nV; j++) {
			char volts[10];
#if defined(WIN32) || defined(WIN64)
			_snprintf(volts, 10, "%4.4f ", vectorAMPA_[i][j]);
#else
			snprintf(volts, 10, "%4.4f ", vectorAMPA_[i][j]);
#endif
			strcat(buffer, volts);
			if (j%dispAmpaPerRow == dispAmpaPerRow-1 && j<nV-1) {
				KERNEL_INFO("%s",buffer);
				strcpy(buffer,"|         |");
			}
		}
		KERNEL_INFO("%s",buffer);
	}

}
