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

#include <normal_spikegen.h>

#include <user_errors.h>	// fancy error messages
#include <algorithm>		// std::find
#include <array>			
#include <cassert>			// assert
#include <cmath>

#define PI               3.1415926535897931e+0

NormalSpikeGenerator::NormalSpikeGenerator(double mean, double sd, int events, int period, bool write_to_file):
	mean_(mean), sd_(sd), events_(events), period_(period)
{
	int n = round(mean * 2 + 1);
	pdf.resize(n);
	heat.resize(n);
	double distribution = .0;
	samples_ = 0;

	int heat_e = 0;  // must be less or equal than neuron size !!! otherwise mathematical not correct 

	// ms
	for (int t = 0; t < n; t++)
	{
		pdf[t] = (1 / (sd * sqrt(2 * PI))) * exp( - pow( ((double)t) - mean, 2) / (2 * pow(sd, 2)) );
		heat[t] = round(pdf[t] * events); 
		heat_e = std::max(heat[t], heat_e);
		//printf("pdf[%2d]=%.3f -> %02d\n", t, pdf[t], heat[t]);
		distribution += pdf[t]; // cdf
		samples_ += heat[t];
	}
	//printf("samples = %d, D=%f, heat_E: %d \n", samples_, distribution, heat_e); 

	currentIndex_ = 0;
}

/* Callback routine SNN::userDefinedSpikeGenerator 

	for(int gNId = groupConfigMDMap[gGrpId].gStartN; gNId <= groupConfigMDMap[gGrpId].gEndN; gNId++) {
		while (!done) {
			int nextSchedTime = spikeGenFunc->nextSpikeTime(this, gGrpId, gNId - groupConfigMDMap[gGrpId].gStartN, currTime, nextTime, endOfTimeWindow);
			if ((nextSchedTime==0 || nextSchedTime>nextTime) && nextSchedTime<endOfTimeWindow && nextSchedTime>=currTime) {
				nextTime = nextSchedTime;
				spikeBuf->schedule(gNId, gGrpId, nextTime - currTime);
			} else {
				done = true;
			}
*/
int NormalSpikeGenerator::nextSpikeTime(CARLsim* sim, int grpId, int nid, int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice) {

	//printf("nid:%u currentTime: %u lastScheduled: %u %u\n", nid, currentTime, lastScheduledSpikeTime, endOfTimeSlice);


	if (period_ > 0) {

		//printf("currentTime %% period_ ==  %d \n", currentTime % period_);

		if (currentTime % period_ == 0) {
			currentIndex_ = currentTime;
		}

		int t = currentTime - currentIndex_;
		if (t < heat.size() && nid < heat[t])
			return currentTime + 1;
		else 
			return 0xFFFFFFFF; // no start
	} 
	else
	{
		if (lastScheduledSpikeTime == 0) {
			// search for first 
			for (int t = 0; t < heat.size(); t++) {
				if (nid < heat[t])
					return t; // become new lastScheduledSpikeTime
			}
			return 0xFFFFFFFF; // no start
		}
		else {
			int t = lastScheduledSpikeTime + 1;   // index
			if (t <  heat.size()) {
				if (nid < heat[t])
					 return t;
			}
			return 0xFFFFFFFF; // no start
		}

		return 0xFFFFFFFF;
	}


}

//void PeriodicSpikeGenerator::checkFiringRate() {
//	UserErrors::assertTrue(rate_>0, UserErrors::MUST_BE_POSITIVE, "PeriodicSpikeGenerator", "Firing rate");
//}
