	auto perfmon = carlsim->setPerformanceMonitor(PMB_MS, "DEFAULT"); 
	perfmon->setPersistentData(true);
	perfmon->setSampleRate(100);

	auto spikemon_0 = carlsim->setSpikeMonitor(0, "DEFAULT");
	spikemon_0->setPersistentData(true);

	auto spikemon_1 = carlsim->setSpikeMonitor(1, "DEFAULT");
	spikemon_1->setPersistentData(true);

	auto spikemon_2 = carlsim->setSpikeMonitor(2, "DEFAULT");
	spikemon_2->setPersistentData(true);

	auto spikemon_3 = carlsim->setSpikeMonitor(3, "DEFAULT");
	spikemon_3->setPersistentData(true);

	auto spikemon_4 = carlsim->setSpikeMonitor(4, "DEFAULT");
	spikemon_4->setPersistentData(true);

	auto spikemon_5 = carlsim->setSpikeMonitor(5, "DEFAULT");
	spikemon_5->setPersistentData(true);

	auto spikemon_6 = carlsim->setSpikeMonitor(6, "DEFAULT");
	spikemon_6->setPersistentData(true);

	auto spikemon_7 = carlsim->setSpikeMonitor(7, "DEFAULT");
	spikemon_7->setPersistentData(true);

	auto spikemon_8 = carlsim->setSpikeMonitor(8, "DEFAULT");
	spikemon_8->setPersistentData(true);

