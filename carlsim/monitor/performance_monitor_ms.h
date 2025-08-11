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

#ifndef _PERFORMANCE_MON_MS_H_
#define _PERFORMANCE_MON_MS_H_


#include <carlsim_datastructures.h>	// NeuronMonMode
#include <stdio.h>					// FILE
#include <vector>					// std::vector

#include <performance_monitor_core.h>

// be specific here  
#if defined(WIN32)   // also defined for WIN64
#include <windows.h>
// C:\Program Files(x86)\Windows Kits\10\Include\10.0.26100.0\um  
// --> SDK ? CMake checks if Windows SDK is installed, same goes for Intel PCM
#include <pdh.h>
#include <pdhmsg.h>
#endif


class SNN; // forward declaration of SNN class

class PerformanceMonitorMs: public PerformanceMonitorCore {
public:
	//! constructor (called by CARLsim::setPerformanceMonitor)
	PerformanceMonitorMs(SNN* snn, int monitorId, int sampleRate);

	//! destructor, cleans up all the memory upon object deletion
	virtual ~PerformanceMonitorMs();

	//void pushMsPdh();  // int time
	void virtual pushPerformanceCounter(); 


 private:

	 //! initialization method
	virtual void pre_init();
	virtual void init();

	virtual void release(); 


#if defined(WIN32) 
	PDH_HQUERY pdhQuery_;
	std::vector<PDH_HCOUNTER> pdhCounter_;
#endif


};
#endif