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

#include "file_connection_generator.h"

#include <stdio.h>
#include <algorithm>		// std::find
#include <assert.h>			// assert

#include <carlsim_log_definitions.h>	// CARLSIM_ERROR, CARLSIM_WARN, CARLSIM_INFO




ConnectionGeneratorFromFile::ConnectionGeneratorFromFile(std::string fileName) {

	memset(&conn_gen_header, 0, sizeof(conn_gen_header_t));

	// open file
	FILE *file = fopen(fileName.c_str(), "rb");
	
	// read header


	//auto nbytes = fread(&conn_gen_header, sizeof(conn_gen_header_t), 1, file);

	auto records = fread(&conn_gen_header, sizeof(conn_gen_header_t), 1, file);
	
	assert(conn_gen_header.version == 100000);
	assert(sizeof(conn_gen_header_t) == conn_gen_header.header_size);

	auto connected = conn_gen_header.connected; 
	auto content = conn_gen_header.content;

	assert(conn_gen_header.payload_size == (sizeof(unsigned) * 2 + sizeof(float) * 2) * content);

	weightFactor = conn_gen_header.weight_factor;
	nfrom = conn_gen_header.nfrom;
	nto = conn_gen_header.nto;

	auto connId = conn_gen_header.gConnId;
	auto gIdPre = conn_gen_header.gIdPre;
	auto gIdPost = conn_gen_header.gIdPost;


	// read payload
	synapses.reserve(content); // * 1.5 due hash
	connections.resize(nfrom * nto);
	key_t key;
	value_t value;
	for (unsigned i = 0; i < content; i++) {
		records = fread(&key, sizeof(key_t), 1, file);
		assert(records);
		records = fread(&value, sizeof(value_t), 1, file);
		assert(records);
		synapses[key] = value;
		connections[idx(key.first, key.second)] = true;
	}

	// validate content
	connected = 0;
	for (unsigned pre = 0; pre < nfrom; pre++)
		for (unsigned post = 0; post < nto; post++) {
			connected += isConnected(pre, post) ? 1 : 0;
			// reserved: checksum (order dependent)
		}
	assert(connected == content);

	// close file
	fclose(file);
}

//! SpikeGeneratorFromFile destructor
ConnectionGeneratorFromFile::~ConnectionGeneratorFromFile() {
}


int ConnectionGeneratorFromFile::idx(unsigned pre, unsigned post) {

	assert(pre >= 0 && pre < nfrom);
	assert(post >= 0 && post < nto);
	return pre * nto + post;
}

bool ConnectionGeneratorFromFile::isConnected(unsigned pre, unsigned post) {
	return connections[idx(pre, post)];
}


//#define DEBUG_CONGRP

void ConnectionGeneratorFromFile::connect(CARLsim* s, int srcGrpId, int i, int destGrpId, int j, float& weight, float& maxWt,
	float& delay, bool& connected) {

	connected = isConnected(i, j);

	if (connected) {
		auto synapse = synapses[std::pair<unsigned, unsigned>(i, j)];
		weight = synapse.first * weightFactor;
		delay = synapse.second;
#ifdef DEBUG_CONGRP
		printf("srcGrp[%d].pre[%d] --> destGrp[%d].post[%d] w:%f  d:%f \n",
			srcGrpId, i, destGrpId, j, weight, delay);
#endif
	}
	else {
		weight = .0f;
		delay = 0.f;
	}

	maxWt = 4.f * weightFactor; // get from Connection Group parameter and store in class
}




