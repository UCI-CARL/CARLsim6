
/*
Neuron Types of synchronous basemodel


Concept: Apply aribtrary Markov process
E_i = (t_i, f_pois, I_c)  -->  (f, ..)_t   t = 1,...  sim_type,  
compare t_i when change happens -> describe as events E_i
*/

#ifndef CA3TYPES_TEST_H
#define CA3TYPES_TEST_H


// CARLsim variable
ComputingBackend BACKEND_CORES;

const enum NeuronTypes { PC, AAC, BC, BCCK, BiC, MFA, QuadD};
const int n_NT = 8; // n Neuron Types
	

// values are derived from generateCONFIGStateSTP.h of CA3 SNN GPU synchonous baseline

// Pyramidal cells with their recurrent connection
auto create_PC = [&](CARLsim* sim, int ng[], const int n = 74366) {
	int CA3_Pyramidal = sim->createGroup("CA3_Pyramidal", n, EXCITATORY_NEURON, -1, BACKEND_CORES);
	sim->setNeuronParameters(CA3_Pyramidal, 366.0, 0.0, 0.792338703789581, 0.0,
		-63.2044008171655, 0.0, -33.6041733124267, 0.0, 0.00838350334098279,
		0.0, -42.5524776883928, 0.0, 35.8614648558726,
		0.0, -38.8680990294091, 0.0,
		588.0, 0.0, 1);
	ng[PC] = CA3_Pyramidal;
};

auto connect_PC_PC = [&](CARLsim* sim, int cg[][n_NT], int ng[], float w = 0.55f, float p = 0.025f) {
	int g_PC_PC = sim->connect(ng[PC], ng[PC], "random", RangeWeight(0.0f, w, 1.55f), p,
                             RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 0.553062478f, 0.0f);
	sim->setSTP(ng[PC], ng[PC], true,
              STPu(0.27922089865f, 0.0f),
              STPtauU(21.44820657f, 0.0f),
              STPtauX(318.510891f, 0.0f),
              STPtdAMPA(10.21893984f, 0.0f),
              STPtdNMDA(150.0f, 0.0f),
              STPtdGABAa(6.0f, 0.0f),
              STPtdGABAb(150.0f, 0.0f),
              STPtrNMDA(0.0f, 0.0f),
              STPtrGABAb(0.0f, 0.0f));	
	cg[PC][PC] = g_PC_PC;
};


// Basket cells with their recurrent connection
auto create_BC = [&](CARLsim* sim, int ng[], const int n = 515) {
	int CA3_Basket = sim->createGroup("CA3_Basket", n,
		INHIBITORY_NEURON, 0, BACKEND_CORES);
	sim->setNeuronParameters(CA3_Basket, 45.0, 0.0, 0.9951729, 0.0,
		-57.506126, 0.0, -23.378766, 0.0, 0.003846186,
		0.0, 9.2642765, 0.0, 18.454934,
		0.0, -47.555661, 0.0,
		-6.0, 0.0, 1);
	ng[BC] = CA3_Basket;
};

auto connect_BC_BC = [&](CARLsim* sim, int cg[][n_NT], int ng[], float w = 0.55f, float p = 0.025f) {
	int g_BC_BC = sim->connect(ng[BC], ng[BC], "random", RangeWeight(0.0f, w, 1.55f), p,
		RangeDelay(1), RadiusRF(-1.0), SYN_PLASTIC, 3.281611994f, 0.0f);
	sim->setSTP(ng[BC], ng[BC], true, 
		STPu(0.38950627465000004f, 0.0f),
		STPtauU(11.19042564f, 0.0f),
		STPtauX(689.5059466f, 0.0f),
		STPtdAMPA(5.0f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(3.007016545f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f));
	cg[BC][BC] = g_BC_BC;
};


// the Pyramidal - Basket sub-ciruit

auto connect_BC_PC = [&](CARLsim* sim, int cg[][n_NT], int ng[], float w = 1.45f, float p = 0.15f) {
	int g_BC_PC = sim->connect(ng[BC], ng[PC], "random", RangeWeight(0.0f, w, 2.45f), p,
		RangeDelay(1), RadiusRF(-1.0), SYN_PLASTIC, 1.572405696f, 0.0f);
	sim->setSTP(ng[BC], ng[PC], true, 
		STPu(0.12521945645000002f, 0.0f),
		STPtauU(16.73589406f, 0.0f),
		STPtauX(384.3363321f, 0.0f),
		STPtdAMPA(5.0f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(7.63862234f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f));
	cg[BC][PC] = g_BC_PC;
};

auto connect_PC_BC = [&](CARLsim* sim, int cg[][n_NT], int ng[], float w = 1.45f, float p = 0.0197417562762975f) {
	//int g_PC_BC = sim->connect(ng[PC], ng[BC], "random", RangeWeight(0.0f, w, 2.45f), p,
	int g_PC_BC = sim->connect(ng[PC], ng[BC], "random", RangeWeight(0.0f, w, 40.0f), p,
		RangeDelay(1, 2), RadiusRF(-1.0), SYN_PLASTIC, 1.172460639f, 0.0f);
	sim->setSTP(ng[PC], ng[BC], true, 
		STPu(0.12174287290000001f, 0.0f),
		STPtauU(21.16086172f, 0.0f),
		STPtauX(691.4177768f, 0.0f),
		STPtdAMPA(3.97130389f, 0.0f),
		STPtdNMDA(150.0f, 0.0f),
		STPtdGABAa(6.0f, 0.0f),
		STPtdGABAb(150.0f, 0.0f),
		STPtrNMDA(0.0f, 0.0f),
		STPtrGABAb(0.0f, 0.0f));
	cg[PC][BC] = g_PC_BC;
};

#endif // CA3TYPES_TEST_H