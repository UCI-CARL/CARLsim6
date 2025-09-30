	int grpId_0;
	{
		auto grpId = carlsim->createSpikeGeneratorGroup("Cstim", 200, EXCITATORY_NEURON);
		carlsim->setSpikeGenerator(grpId, spikegen_0);
		assert(grpId == 0);
		grpId_0 = grpId;
	}

	int grpId_1;
	{
		auto grpId = carlsim->createGroup("Cexc0", 200, EXCITATORY_NEURON, 0, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 1);
		grpId_1 = grpId;
	}

	int grpId_2;
	{
		auto grpId = carlsim->createGroup("Cinh0", 50, INHIBITORY_NEURON, 0, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.100000, 0.200000, -65.000000, 2.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 2);
		grpId_2 = grpId;
	}

	int grpId_3;
	{
		auto grpId = carlsim->createGroup("Cexc1", 200, EXCITATORY_NEURON, 1, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 3);
		grpId_3 = grpId;
	}

	int grpId_4;
	{
		auto grpId = carlsim->createGroup("Cinh1", 50, INHIBITORY_NEURON, 1, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.100000, 0.200000, -65.000000, 2.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 4);
		grpId_4 = grpId;
	}

	int grpId_5;
	{
		auto grpId = carlsim->createGroup("Cexc2", 200, EXCITATORY_NEURON, 2, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 5);
		grpId_5 = grpId;
	}

	int grpId_6;
	{
		auto grpId = carlsim->createGroup("Cinh2", 50, INHIBITORY_NEURON, 2, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.100000, 0.200000, -65.000000, 2.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 6);
		grpId_6 = grpId;
	}

	int grpId_7;
	{
		auto grpId = carlsim->createGroup("Cexc3", 200, EXCITATORY_NEURON, 3, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 7);
		grpId_7 = grpId;
	}

	int grpId_8;
	{
		auto grpId = carlsim->createGroup("Cinh3", 50, INHIBITORY_NEURON, 3, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.100000, 0.200000, -65.000000, 2.000000);
		carlsim->setConductances(grpId, false);
		assert(grpId == 8);
		grpId_8 = grpId;
	}

