	auto spikemon_0 = carlsim->setSpikeMonitor(0, "DEFAULT");
	spikemon_0->setPersistentData(true);

	auto spikemon_1 = carlsim->setSpikeMonitor(1, "DEFAULT");
	spikemon_1->setPersistentData(true);

	auto spikemon_2 = carlsim->setSpikeMonitor(2, "DEFAULT");
	spikemon_2->setPersistentData(true);

	auto neuronmon_1 = carlsim->setNeuronMonitor(1, "DEFAULT");
	neuronmon_1->setPersistentData(true);

	auto cobamon_1 = carlsim->setCobaMonitor(1, "DEFAULT");
	cobamon_1->setPersistentData(true);

