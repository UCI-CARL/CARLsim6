/** Copyright (c) 2016 Regents of the University of California. All rights reserved.
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

#ifndef _COBA_MON_CORE_H_
#define _COBA_MON_CORE_H_


#include <carlsim_datastructures.h>	// NeuronMonMode
#include <stdio.h>					// FILE
#include <vector>					// std::vector

class SNN; // forward declaration of SNN class

class CobaMonitorCore {
public:
	//! constructor (called by CARLsim::setCobaMonitor)
	CobaMonitorCore(SNN* snn, int monitorId, int grpId);

	//! destructor, cleans up all the memory upon object deletion
	~CobaMonitorCore();

    //! returns the Neuron state vector
	//! issue const ref
	std::vector<std::vector<float> > getVectorAMPA();
	std::vector<std::vector<float> > getVectorNMDA();
	std::vector<std::vector<float> > getVectorGABAa();
	std::vector<std::vector<float> > getVectorGABAb();

    //! returns recording status
	bool isRecording() { return recordSet_; }

    //! inserts a (time,neurId) tupel into the D Neuron State vector
	void pushCoba(int neurId, float AMPA, float NMDA, float GABAa, float GABAb);

    //! starts recording Neuron state
	void startRecording();

	//! stops recording Neuron state
	void stopRecording();

    //! deletes data from the neuron state vector
	void clear();

    //! sets pointer to COBA file
	void setCobaFileId(FILE* cobaFileId);

	//! returns a pointer to the coba file
	FILE* getCobaFileId() { return cobaFileId_; }


	//{ LN20201118 extensions

	//! returns status of PersistentData mode	
	bool getPersistentData() { return persistentData_; }

	//! sets status of PersistentData mode
	void setPersistentData(bool persistentData) { persistentData_ = persistentData; }

	// }


    //! returns timestamp of last CobaMonitor update
	long int getLastUpdated() { return cobaMonLastUpdated_; }

	//! sets timestamp of last CobaMonitor update
	void setLastUpdated(long int lastUpdate) { cobaMonLastUpdated_ = lastUpdate; }

	//! returns true if state buffers are close to maxAllowedBufferSize
    bool isBufferBig();

    //! returns the approximate size of the state vectors in bytes
    long int getBufferSize();

    //! returns the total accumulated time
    long int getAccumTime();

	void writeCobaFileHeader();

	//! prints neuron states in human-readable format
	void print(bool meanOnly);

 private:
    //! initialization method
	void init();

    //! whether we have to write header section of neuron file
	bool needToWriteFileHeader_;

    SNN* snn_;	//!< private CARLsim implementation
	int monitorId_;	//!< current CobaMonitor ID
	int grpId_;		//!< current group ID
	int nNeurons_;	//!< number of neurons in the group

	FILE* cobaFileId_;	//!< file pointer to the coba state file or NULL
	int cobaFileSignature_; //!< int signature of coba file
	float cobaFileVersion_; //!< version number of coba file

	//! Used to analyzed the neuron state information
	std::vector<std::vector<float> > vectorAMPA_;
	std::vector<std::vector<float> > vectorNMDA_;
	std::vector<std::vector<float> > vectorGABAa_;
	std::vector<std::vector<float> > vectorGABAb_;

	bool recordSet_;			//!< flag that indicates whether we're currently recording
	long int startTime_;	 	//!< time (ms) of first call to startRecording
	long int startTimeLast_; 	//!< time (ms) of last call to startRecording
	long int stopTime_;		 	//!< time (ms) of stopRecording
	long int totalTime_;		//!< the total amount of recording time (over all recording periods)
	long int accumTime_;

	long int cobaMonLastUpdated_;//!< time (ms) when group was last run through updateCobaMonitor

	//! whether data should be persistent (true) or clear() should be automatically called by startRecording (false)
	bool persistentData_;

    //! Indicates if we have returned true at least once in isBufferBig(). Gets reset in stopRecording(). Used to warn the user only once.
	bool userHasBeenWarned_;

	// file pointers for error logging
	const FILE* fpInf_;
	const FILE* fpErr_;
	const FILE* fpDeb_;
	const FILE* fpLog_;
};
#endif