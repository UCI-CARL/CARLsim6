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

#ifndef _NEURON_MON_H_
#define _NEURON_MON_H_

#include "carlsim_api.h"

#include <carlsim_datastructures.h> // NeuronMonMode
#include <vector>					// std::vector

class SNN; 			// forward declaration of SNN class
class NeuronMonitorCore; // forward declaration of implementation

class CARLSIM_API NeuronMonitor {
 public:
	/*!
	 * \brief NeuronMonitor constructor
	 *
	 * Creates a new instance of the NeuronMonitor class.
	 *
	 */
	NeuronMonitor(NeuronMonitorCore* neuronMonitorCorePtr);

	/*!
	 * \brief NeuronMonitor destructor.
	 *
	 * Cleans up all the memory upon object deletion.
	 *
	 */
	~NeuronMonitor();

    void clear();
    bool isRecording();
    void startRecording();
    void stopRecording();
    void setLogFile(const std::string& logFileName);
	void print(bool meanOnly = false);

	//{ LN20201118 extensions

	/*!
	* \brief Returns a flag that indicates whether PersistentMode is on (true) or off (false)
	*
	* This function returns a flag that indicates whether PersistentMode is currently on (true) or off (false).
	* If PersistentMode is off, only the last recording period will be considered for calculating metrics.
	* If PersistentMode is on, all the recording periods will be considered. By default, PersistentMode is off, but
	* can be switched on at any point in time by calling setPersistentData(bool).
	*/
	bool getPersistentData();

	/*!
	* \brief Sets PersistentMode either on (true) or off (false)
	*
	* This function sets PersistentMode either on (true) or off (false).
	* If PersistentMode is off, only the last recording period will be considered for calculating metrics.
	* If PersistentMode is on, all the recording periods will be considered. By default, PersistentMode is off, but
	* can be switched on at any point in time.
	* The current state of PersistentMode can be retrieved by calling getPersistentData().
	*/
	void setPersistentData(bool persistentData);

	/*!
	* \brief Returns the ...
	*
	* This function returns 
	*
	*/
	int getLastUpdated();

	//! returns the Neuron state vectors
	std::vector<std::vector<float> > getVectorV();
	std::vector<std::vector<float> > getVectorU();
	std::vector<std::vector<float> > getVectorI();

	//}

 private:
  //! This is a pointer to the actual implementation of the class. The user should never directly instantiate it.
  NeuronMonitorCore* neuronMonitorCorePtr_;

};
#endif