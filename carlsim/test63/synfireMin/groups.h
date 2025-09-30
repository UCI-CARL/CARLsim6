	int grpId_0;
	{
		auto grpId = carlsim->createSpikeGeneratorGroup("Gstim", 100, EXCITATORY_NEURON);
		carlsim->setSpikeGenerator(grpId, spikegen_0);
		assert(grpId == 0);
		grpId_0 = grpId;
	}

	int grpId_1;
	{
		auto grpId = carlsim->createGroup("Gexc0", 1, EXCITATORY_NEURON, -1, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.020000, 0.200000, -65.000000, 8.000000);
		carlsim->setConductances(grpId, true, 2, 2, 5, 10);
		assert(grpId == 1);
		grpId_1 = grpId;
	}

	int grpId_2;
	{
		auto grpId = carlsim->createGroup("Ginh0", 25, INHIBITORY_NEURON, -1, (ComputingBackend)0);
		carlsim->setNeuronParameters(grpId, 0.100000, 0.200000, -65.000000, 2.000000);
		carlsim->setConductances(grpId, true, 2, 2, 5, 10);
		assert(grpId == 2);
		grpId_2 = grpId;
	}

