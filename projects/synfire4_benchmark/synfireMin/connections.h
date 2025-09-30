	int conn_id_0;
	ConnectionGeneratorFromFile* conngen_0 = nullptr;
	{
		conngen_0 = new ConnectionGeneratorFromFile("synfireMin/conngrpgen_0_0_1.dat");
		auto connId = carlsim->connect(0, 1, conngen_0, SYN_FIXED);
		assert(connId == 0);
		conn_id_0 = connId;
	}

	int conn_id_1;
	ConnectionGeneratorFromFile* conngen_1 = nullptr;
	{
		conngen_1 = new ConnectionGeneratorFromFile("synfireMin/conngrpgen_1_0_2.dat");
		auto connId = carlsim->connect(0, 2, conngen_1, SYN_FIXED);
		assert(connId == 1);
		conn_id_1 = connId;
	}

	int conn_id_2;
	ConnectionGeneratorFromFile* conngen_2 = nullptr;
	{
		conngen_2 = new ConnectionGeneratorFromFile("synfireMin/conngrpgen_2_2_1.dat");
		auto connId = carlsim->connect(2, 1, conngen_2, SYN_FIXED);
		assert(connId == 2);
		conn_id_2 = connId;
	}

