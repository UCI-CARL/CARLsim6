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

#ifndef _EXECUTION_STOPWATCH_H_
#define _EXECUTION_STOPWATCH_H_

#ifdef __NO_CUDA__

#include "carlsim_api.h"

#include <chrono> // std::chrono::steady_clock::time_point

/*!
 * \brief A stopwatch class for measuring program execution time
 * 
 * This class implements a class for a stopwatch object that can be used to measure the runtime for
 * a program or a part of it when CARLsim is not run on the GPU. The stopwatch is started at the
 * beginning of the part of the program to be measured and stopped at the end. After a measurement,
 * the stopwatch can be used to get the time elapsed and reset for a new measurement.
 */
class CARLSIM_API ExecutionStopwatch
{
private:
	bool _timer_on; //!< true if the timer is on, false if off
	std::chrono::steady_clock::time_point _start_time; //!< time point when the stopwatch is started
	std::chrono::steady_clock::time_point _stop_time; //!< time point when the stopwatch is stopped

public:
	/*!
	 * \brief Starts the stopwatch by recording the time it was started
	 */
	void start();

	/*!
	 * \brief Stops the stopwatch by recording the time it was stopped
	 */
	void stop();

	/*!
	 * \brief Gets the time measured by the stopwatch (time between start and stop time)
	 * 
	 * This function returns the time measured by the stopwatch, specifically the difference between
	 * the start and stop time. If the timer has not been stopped, a time of 0 will be returned.
	 * 
	 * \returns the time measured by the stopwatch (s)
	 */
	float get_time();

	/*!
	 * \brief Resets the stopwatch by clearing the currently stored start and stop times
	 */
	void reset();
};
#endif

#endif // _EXECUTION_STOPWATCH_H_