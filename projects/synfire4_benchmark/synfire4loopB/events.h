	// 0 ms
	perfmon->startRecording();

	// 0 ms
	spikemon_0->startRecording();

	// 0 ms
	spikemon_1->startRecording();

	// 0 ms
	spikemon_2->startRecording();

	// 0 ms
	spikemon_3->startRecording();

	// 0 ms
	spikemon_4->startRecording();

	// 0 ms
	spikemon_5->startRecording();

	// 0 ms
	spikemon_6->startRecording();

	// 0 ms
	spikemon_7->startRecording();

	// 0 ms
	spikemon_8->startRecording();

	// 0 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {{3,80.000000}, {69,80.000000}, {130,80.000000}};
		for(auto iter=aer.begin(); iter!=aer.end(); iter++) {vect[iter->first] = iter->second;};
		carlsim->setExternalCurrent(1, vect);
	}

	// 0 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {{117,80.000000}};
		for(auto iter=aer.begin(); iter!=aer.end(); iter++) {vect[iter->first] = iter->second;};
		carlsim->setExternalCurrent(3, vect);
	}

	// 0 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {};
		carlsim->setExternalCurrent(5, vect);
	}

	// 0 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {};
		carlsim->setExternalCurrent(7, vect);
	}

	// 1 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {{123,80.000000}, {126,80.000000}};
		for(auto iter=aer.begin(); iter!=aer.end(); iter++) {vect[iter->first] = iter->second;};
		carlsim->setExternalCurrent(1, vect);
	}

	// 1 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {};
		carlsim->setExternalCurrent(3, vect);
	}

	// 1 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {};
		carlsim->setExternalCurrent(5, vect);
	}

	// 1 ms
	{
		std::vector<float> vect(200, .0f);
		std::vector<std::pair<int,float>> aer = {{74,80.000000}, {83,80.000000}, {99,80.000000}};
		for(auto iter=aer.begin(); iter!=aer.end(); iter++) {vect[iter->first] = iter->second;};
		carlsim->setExternalCurrent(7, vect);
	}

