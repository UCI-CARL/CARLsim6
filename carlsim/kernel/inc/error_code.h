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

#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

#define NO_KERNEL_ERRORS		 			0xc00d

#define NEW_FIRE_UPDATE_OVERFLOW_ERROR1  	0x61
#define NEW_FIRE_UPDATE_OVERFLOW_ERROR2  	0x62

#define STORE_FIRING_ERROR_0 				0x54

#define ERROR_FIRING_0 						0xd0d0
#define ERROR_FIRING_1 						0xd0d1
#define ERROR_FIRING_2 						0xd0d2
#define ERROR_FIRING_3 						0xd0d3

#define GLOBAL_STATE_ERROR_0  				0xf0f0

#define GLOBAL_CONDUCTANCE_ERROR_0  		0xfff0
#define GLOBAL_CONDUCTANCE_ERROR_1			0xfff1

#define STP_ERROR 							0xf000

#define UPDATE_WEIGHTS_ERROR1				0x80

#define CURRENT_UPDATE_ERROR1   			0x51
#define CURRENT_UPDATE_ERROR2   			0x52
#define CURRENT_UPDATE_ERROR3   			0x53
#define CURRENT_UPDATE_ERROR4   			0x54

#define KERNEL_CURRENT_ERROR0  				0x90
#define KERNEL_CURRENT_ERROR1  				0x91
#define KERNEL_CURRENT_ERROR2  				0x92

#define ICURRENT_UPDATE_ERROR1   			0x51
#define ICURRENT_UPDATE_ERROR2   			0x52
#define ICURRENT_UPDATE_ERROR3   			0x53
#define ICURRENT_UPDATE_ERROR4   			0x54
#define ICURRENT_UPDATE_ERROR5   			0x55

#define KERNEL_INIT_ERROR0					0x71
#define KERNEL_INIT_ERROR1					0x72

#define POISSON_COUNT_ERROR_0				0x99

#define UNKNOWN_LOGGER_ERROR				0x8001
#define NO_LOGGER_DIR_ERROR					0x8002

#define ID_OVERFLOW_ERROR					0x8003



// \todo review by Jinwei

// -1 ERRORS

//KERNEL_ERROR("Invalid connection type (should be 'random', 'full', 'one-to-one', 'full-no-direct', or 'gaussian')");
#define KERNEL_ERROR_INVALID_CONN				0xFF001

//KERNEL_ERROR("STDP requires plastic connection");
#define KERNEL_ERROR_STDP_SYN_PLASIC			0xFF002

//KERNEL_ERROR("Invalid connection type( should be 'random', 'full', 'full-no-direct', or 'one-to-one')");
#define KERNEL_ERROR_INVALID_CONN2				0xFF003

//KERNEL_ERROR("Invalid connection type( should be 'random', 'full', 'full-no-direct', or 'one-to-one')");
#define KERNEL_ERROR_INVALID_CONN3				0xFF004

//KERNEL_ERROR("Can't find catagory for the group [%d] ", grpIt->gGrpId);
#define KERNEL_ERROR_NO_CATAGORY 				0xFF005

//KERNEL_ERROR("Unkown simulation mode");
#define KERNEL_ERROR_UNKNOWN_SIM_MODE 			0xFF006

//KERNEL_ERROR("Can't assign the group [%d] to any partition", grpIt->second.gGrpId);
#define KERNEL_ERROR_NO_PARTION_ASSIGNED 		0xFF007

//KERNEL_ERROR("loadSimulation: Unknown file signature. This does not seem to be a "
#define KERNEL_ERROR_UNKNOWN_FILE_SIGNATURE		0xFF008

// KERNEL_ERROR("loadSimulation: Unsupported version number (%f)",tmpFloat);
#define KERNEL_ERROR_UNSUPPORTED_VERSION		0xFF009

// KERNEL_ERROR("loadSimulation: Number of neurons in file (%d) and simulation (%d) don't match.",
#define KERNEL_ERROR_NEURON_MISMATCH			0xFF010

//KERNEL_ERROR("loadSimulation: Number of groups in file (%d) and simulation (%d) don't match.",
#define KERNEL_ERROR_GROUP_MISMATCH				0xFF011

//fprintf(stderr,"loadSimulation: Error while reading file header");  // \todo Jinwei: why fprintf instead of KERNEL_ERROR
#define KERNEL_ERROR_INVALID_HEADER				0xFF012

//KERNEL_ERROR("loadSimulation: StartN in file (%d) and grpInfo (%d) for group %d don't match.",
#define KERNEL_ERROR_INVALID_START				0xFF013

//KERNEL_ERROR("loadSimulation: EndN in file (%d) and grpInfo (%d) for group %d don't match.",
#define KERNEL_ERROR_INVALID_END				0xFF014

//KERNEL_ERROR("loadSimulation: numX in file (%d) and grpInfo (%d) for group %d don't match.",
#define KERNEL_ERROR_NUMX						0xFF015

//KERNEL_ERROR("loadSimulation: numY in file (%d) and grpInfo (%d) for group %d don't match.",
#define KERNEL_ERROR_NUMY						0xFF016

//KERNEL_ERROR("loadSimulation: numZ in file (%d) and grpInfo (%d) for group %d don't match.",
#define KERNEL_ERROR_NUMZ						0xFF017

//KERNEL_ERROR("loadSimulation: Group names in file (%s) and grpInfo (%s) don't match.", name,
#define KERNEL_ERROR_GROUP_NAMES_MISMATCH		0xFF018

//KERNEL_ERROR("loadSimulation: Error while reading group info");
#define KERNEL_ERROR_INVALID_GROUP_INFO			0xFF019

//KERNEL_ERROR("loadSimulation: source group in file (%d) and in simulation (%d) for connection %d don't match.",
#define KERNEL_ERROR_SRC_GRP_CONN				0xFF020

//KERNEL_ERROR("loadSimulation: dest group in file (%d) and in simulation (%d) for connection %d don't match.",
#define KERNEL_ERROR_DEST_GRP_CONN				0xFF021

//KERNEL_ERROR("setNeuronParameters must be called for group %s (G:%d,L:%d)",groupConfigMap[gGrpId].grpName.c_str(), gGrpId, lGrpId);
#define KERNEL_ERROR_IZH_PARAMS_NOT_SET			0xFF022

//KERNEL_ERROR("setNeuronParametersLIF must be called for group %s (G:%d,L:%d)",groupConfigMap[gGrpId].grpName.c_str(), gGrpId, lGrpId);
#define KERNEL_ERROR_LIF_PARAMS_NOT_SET			0xFF023

//KERNEL_ERROR("IcalcType is unknwon in group [%d] ", gGrpId);
#define KERNEL_ERROR_UNKNOWN_ICALC				0xFF024


// 1 ERRORS

//KERNEL_ERROR("Wrong Host-Device copy direction");
#define KERNEL_ERROR_GPU_WRONG_COPY			0xFE001

//KERNEL_ERROR("GPU Memory already allocated...");
#define KERNEL_ERROR_GPU_REALLOC			0xFE002

//case UNKNOWN_INTEGRATION:
#define KERNEL_ERROR_UNKNOWN_INTEG			0xFE003

//KERNEL_ERROR("Invalid type using createGroup... Cannot create poisson generators here.");
#define KERNEL_ERROR_POISSON_1				0xFE004

//KERNEL_ERROR("Invalid type using createGroup... Cannot create poisson generators here.");
#define KERNEL_ERROR_POISSON_2				0xFE005

//KERNEL_ERROR("No connection found from group %d(%s) to group %d(%s)", preGrpId, getGroupName(preGrpId).c_str(),
#define KERNEL_ERROR_CONN_MISSING			0xFE006

//KERNEL_ERROR("No connection found from group %d(%s) to group %d(%s)", preGrpId, getGroupName(preGrpId).c_str(),
#define KERNEL_ERROR_CONN_MISSING2			0xFE007

//KERNEL_ERROR("No connection found from group %d(%s) to group %d(%s)", preGrpId, getGroupName(preGrpId).c_str(),
#define KERNEL_ERROR_CONN_MISSING3			0xFE008

//KERNEL_ERROR("setGroupMonitor has already been called on Group %d (%s).", gGrpId, groupConfigMap[gGrpId].grpName.c_str());
#define KERNEL_ERROR_GROUPMON_SET			0xFE009

//KERNEL_ERROR("No connection found from group %d(%s) to group %d(%s)", grpIdPre, getGroupName(grpIdPre).c_str(),
#define KERNEL_ERROR_CONN_MISSING4			0xFE010

//KERNEL_ERROR("setConnectionMonitor has already been called on Connection %d (MonitorId=%d)", connId, connectConfigMap[connId].connectionMonitorId);
#define KERNEL_ERROR_CONNMON_SET			0xFE011

//KERNEL_ERROR("Connection ID was not found.  Quitting.");
#define KERNEL_ERROR_CONN_MISSING5			0xFE012

//KERNEL_ERROR("Group %s(%d) exceeds max number of allowed compartmental connections (%d).",
#define KERNEL_ERROR_COMP_CONNS_EXCEEDED	0xFE013

//KERNEL_ERROR("Group %s(%d) exceeds max number of allowed compartmental connections (%d).",
#define KERNEL_ERROR_COMP_CONNS_EXCEEDED2	0xFE014

//KERNEL_ERROR("STP with delays > 1 ms is currently not supported.");
#define KERNEL_ERROR_MAX_STP_DELAY			0xFE015

//KERNEL_ERROR("You are using a synaptic delay (%d) greater than MAX_SYN_DELAY defined in config.h", glbNetworkConfig.maxDelay);
#define KERNEL_ERROR_MAX_SYN_DELAY			0xFE016

//KERNEL_ERROR("Group %s(%d) is not compartmentally enabled, cannot be part of a compartmental connection.",
#define KERNEL_ERROR_COMPARTMENT_DISABLED	0xFE017

//KERNEL_ERROR("Group %s(%d) is not compartmentally enabled, cannot be part of a compartmental connection.",
#define KERNEL_ERROR_COMPARTMENT_DISABLED2	0xFE018

//KERNEL_ERROR("If STDP on group %d (%s) is set, group must have some incoming plastic connections.",
#define KERNEL_ERROR_STDP_NO_IN_PLASTIC		0xFE019

//KERNEL_ERROR("If homeostasis is enabled on group %d (%s), then STDP must be enabled, too.",
#define KERNEL_ERROR_STDP_HOMEO_INCONSIST	0xFE020



#endif
