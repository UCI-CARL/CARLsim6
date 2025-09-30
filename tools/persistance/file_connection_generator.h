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
#ifndef FILECONNGEN_H
#define FILECONNGEN_H

#include "callback.h"
//#include "callback_core.h"

#include "carlsim_persistance_api.h"

//#include <stdint.h>
//#include <vector>	// std::vector


#include <string>	// std::string
//#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
//#include <hash_map>
#include <unordered_map>
#include <utility> // for std::pair
#include <tuple>

// -> CARLsim  tool.  loader, 
struct conn_gen_header_t {
	unsigned version; // 01.00.000   Major, Minor, Patch
	size_t header_size;
	size_t payload_size;
	int gConnId;
	int gIdPre;
	int gIdPost;
	unsigned connected;
	unsigned content; // records
	double weight_factor;
	unsigned nfrom;
	unsigned nto;
	unsigned long long checksum; // reserved
};

class CARLSIM_PERSISTANCE_API ConnectionGeneratorFromFile : public ConnectionGenerator {
public:
	/*!
	 * \brief SpikeGeneratorFromFile constructor
	 *
	 * \param[in] fileName file name of spike file (must be created from SpikeMonitor)
	 * \param[in] offsetTimeMs optional offset (ms) that will be applied to all scheduled spike times. Can assume
	 *                         both positive and negative values. Default: 0.
	 */
	ConnectionGeneratorFromFile(std::string fileName);

	//! SpikeGeneratorFromFile destructor
	~ConnectionGeneratorFromFile();

	void connect(CARLsim* s, int srcGrpId, int i, int destGrpId, int j, float& weight, float& maxWt,
		float& delay, bool& connected);

	void setWeightFactor(float factor = 1.0f) { weightFactor = factor; }

	bool isConnected(unsigned pre, unsigned post);

	// serialize the content of synapses to file, with a header describing the payload
	//void writeTo(QString path, unsigned connId, unsigned gIdPre, unsigned gIdPost, bool learning);

	//void readFrom(std::string path);

public: // for testing,  friend accessor



	//const conn_gen_header_t& getHeader() { return conn_gen_header; };

private:
	std::vector<bool> connections;   //! (pre,post)->bool

	int nfrom, nto;
	int idx(unsigned pre, unsigned post);

	// Hash function for std::pair<unsigned, unsigned>
	struct pair_hash {
		std::size_t operator()(const std::pair<unsigned, unsigned>& p) const {
			return std::hash<unsigned>()(p.first) ^ (std::hash<unsigned>()(p.second) << 1);
		}
	};

	//QHash< QPair<unsigned, unsigned>, QPair<float, float>> synapses; //! (pre,post)->(w,d)
	typedef std::pair<unsigned, unsigned> key_t;
	typedef std::pair<float, float > value_t;
	std::unordered_map<key_t, value_t, pair_hash> synapses;

		
	float weightFactor; // adapt CUBA, COBA

	conn_gen_header_t conn_gen_header;
};

#endif // FILECONNGEN_H