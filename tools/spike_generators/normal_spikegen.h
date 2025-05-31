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
#ifndef _NORMAL_SPIKEGEN_H_
#define _NORMAL_SPIKEGEN_H_

#include "carlsim_spike_generators_api.h"

#include <callback.h>
#include <vector>

/*!
 * \brief a normal distributed SpikeGenerator ...
 *
 * This class implements a ...
 */
class CARLSIM_SPIKE_GENERATORS_API NormalSpikeGenerator : public SpikeGenerator {
public:
	/*!
	 * \brief NormalSpikeGenerator constructor
	 * \param[in] mean  in ms 
	 * \param[in] sd    in ms
	 * \param[in] events  corresponds to neuron index in the group, N(E,sd)*events < max(nid)
	 * \param[in] write_to_file  write stimulus package as AER file
	 */
	NormalSpikeGenerator(double mean = 10, double sd = 2.4, int events=400, int period = 0, bool write_to_file = false);  
		// issue: write_to_file requires SpikeGen, .. this might be in conflict with outer modelling 
		// this requires to fully incapsulate the handling, including reading from stored signal packets 
		// instead of generating them  -> Constructor read_from_file, path
		// -> instead of bool pass the pass, if defined, write the file
		// maybe offline without CARLsim -> Static see callback routine this would only require a pseudo group 
		// so three modes:  
		// 1. Transient, each time from scratch,  
		// 2. like 1. but additionally dump file  
		// 3. Standalone to decouple the run from the modelling 

	//! NormalSpikeGenerator destructor
	~NormalSpikeGenerator() {}

	/*!
	 * \brief schedules the next spike time
	 *
	 * This function schedules the next spike time, given the currentTime and the lastScheduledSpikeTime. It implements
	 * the virtual function of the base class.
	 * \param[in] sim pointer to a CARLsim object
	 * \param[in] grpId current group ID for which to schedule spikes
	 * \param[in] nid current neuron ID for which to schedule spikes
	 * \param[in] currentTime current time (ms) at which spike scheduler is called
	 * \param[in] lastScheduledSpikeTime the last time (ms) at which a spike was scheduled for this nid, grpId
	 * \returns the next spike time (ms)
	 */
	int nextSpikeTime(CARLsim* sim, int grpId, int nid, int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice);

	inline const double mean() const { return mean_;  }
	inline const double sd() const { return sd_; }
	inline const int events() const { return samples_; }
	inline const int samples() const { return samples_; }



private:
	//void checkFiringRate();

	// -> refactor to double when covered

	float mean_;		//!< 
	float sd_;		//!< 
	int events_;		//!< spike events 
	int period_;

	int samples_;  //!< effective samples, esp ~ < 1/events * something,  [0,10)

	std::vector<double> pdf;    // -> float  when covered
	std::vector<int> heat; 
	int currentIndex_;
};

#endif    # NORMAL