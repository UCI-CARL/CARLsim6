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

#ifndef _PERFORMANCE_MON_CORE_H_
#define _PERFORMANCE_MON_CORE_H_


#include <carlsim_datastructures.h>	// NeuronMonMode
#include <stdio.h>					// FILE
#include <vector>					// std::vector


class SNN; // forward declaration of SNN class

class PerformanceMonitorCore {
public:
	//! factory 
	static PerformanceMonitorCore* create(SNN* snn, int monitorId, PerformanceMonitorBackend backend = PMB_INTEL, int sampleRate = 1);

	//! constructor (called by CARLsim::setPerformanceMonitor), sampleRate in ms, derived class defines the backend type
	PerformanceMonitorCore(SNN* snn, int monitorId, int sampleRate);  

	//! destructor, cleans up all the memory upon object deletion
	virtual ~PerformanceMonitorCore();


	const int getCores() { return nCores_; }

	// Performance Counter Measurement Implementation
	const std::vector<std::vector<float>> &getUtilization();   // core state active state residency, 0..1
	const std::vector<std::vector<float>> &getInstructions();  // instructions per CPU cycle
	const std::vector<std::vector<float>> &getFrequency();		// core frequency in GHz
	const std::vector<std::vector<float>> &getEnergy();			// energy in Joules derived from the socket by the core activity

	//! returns recording status
	bool isRecording() { return recordSet_; }

	//! default 1ms, set in the constructor as parameter
	int getSampleRate() { return sampleRate_; }

	void setSampleRate(int ms) { sampleRate_ = ms; }

	// --> 
	void virtual pushPerformanceCounter() = 0; // {};  // NOP alternative   = 0 => abstract 

	//! starts recording Neuron state
	void startRecording();

	//! stops recording Neuron state
	void stopRecording();

	//! deletes data from the neuron state vector
	void clear();

	//! sets pointer to COBA file
	void setPerformanceFileId(FILE* performanceFileId);

	//! returns a pointer to the performance file
	FILE* getPerformanceFileId() { return performanceFileId_; }


	//{ LN20201118 extensions

	//! returns status of PersistentData mode	
	bool getPersistentData() { return persistentData_; }

	//! sets status of PersistentData mode
	void setPersistentData(bool persistentData) { persistentData_ = persistentData; }

	// }


	//! returns timestamp of last PerformanceMonitor update
	long int getLastUpdated() { return performanceMonLastUpdated_; }

	//! sets timestamp of last PerformanceMonitor update
	void setLastUpdated(long int lastUpdate) { performanceMonLastUpdated_ = lastUpdate; }

	//! returns true if state buffers are close to maxAllowedBufferSize
	bool isBufferBig();

	//! returns the approximate size of the state vectors in bytes
	long int getBufferSize();

	//! returns the total accumulated time
	long int getAccumTime();

	void writePerformanceFileHeader();

	//! prints neuron states in human-readable format
	void print(bool meanOnly);

protected:
	//! initialization method
	virtual void pre_init() {};
	virtual void init();
	virtual void post_init() {};

	virtual void release() {};


    //! whether we have to write header section of neuron file
	bool needToWriteFileHeader_;

    SNN* snn_;	//!< private CARLsim implementation
	int monitorId_;	//!< current PerformanceMonitor ID
	int sockets_;	//!< current group ID
	int nCores_;	//!< total number of cores in the performance
		// V2: indexed by sockets_ e.g. 32 core Xeon, 2 sockets, 
		// core_i = 20 = socket[1].core[3]

	//! in ms, default 1ms, valid values are 1ms to 100ms 
	int sampleRate_;

	FILE* performanceFileId_;	//!< file pointer to the performance state file or NULL
	int performanceFileSignature_; //!< int signature of performance file
	float performanceFileVersion_; //!< version number of performance file

	std::vector <unsigned long long> time; // ms

	std::vector<std::vector<float> > vectorUtilization_;	// core state active state residency, 0..1
	std::vector<std::vector<float> > vectorInstructions_;	// instructions per CPU cycle
	std::vector<std::vector<float> > vectorFrequency_;	// core frequency in Ghz
	std::vector<std::vector<float> > vectorEnergy_;		// energy in Joules derived from the socket by the core activity


	bool recordSet_;			//!< flag that indicates whether we're currently recording
	long int startTime_;	 	//!< time (ms) of first call to startRecording
	long int startTimeLast_; 	//!< time (ms) of last call to startRecording
	long int stopTime_;		 	//!< time (ms) of stopRecording
	long int totalTime_;		//!< the total amount of recording time (over all recording periods)
	long int accumTime_;

	long int performanceMonLastUpdated_;//!< time (ms) when group was last run through updatePerformanceMonitor

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