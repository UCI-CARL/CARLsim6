	int conn_id_0;
	ConnectionGeneratorFromFile* conngen_0 = nullptr;
	{
		conngen_0 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_0_0_1.dat");
		auto connId = carlsim->connect(0, 1, conngen_0, SYN_FIXED);
		assert(connId == 0);
		conn_id_0 = connId;
	}

	int conn_id_1;
	ConnectionGeneratorFromFile* conngen_1 = nullptr;
	{
		conngen_1 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_1_0_2.dat");
		auto connId = carlsim->connect(0, 2, conngen_1, SYN_FIXED);
		assert(connId == 1);
		conn_id_1 = connId;
	}

	int conn_id_2;
	ConnectionGeneratorFromFile* conngen_2 = nullptr;
	{
		conngen_2 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_2_2_1.dat");
		auto connId = carlsim->connect(2, 1, conngen_2, SYN_FIXED);
		assert(connId == 2);
		conn_id_2 = connId;
	}

	int conn_id_3;
	ConnectionGeneratorFromFile* conngen_3 = nullptr;
	{
		conngen_3 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_3_1_3.dat");
		auto connId = carlsim->connect(1, 3, conngen_3, SYN_FIXED);
		assert(connId == 3);
		conn_id_3 = connId;
	}

	int conn_id_4;
	ConnectionGeneratorFromFile* conngen_4 = nullptr;
	{
		conngen_4 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_4_1_4.dat");
		auto connId = carlsim->connect(1, 4, conngen_4, SYN_FIXED);
		assert(connId == 4);
		conn_id_4 = connId;
	}

	int conn_id_5;
	ConnectionGeneratorFromFile* conngen_5 = nullptr;
	{
		conngen_5 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_5_4_3.dat");
		auto connId = carlsim->connect(4, 3, conngen_5, SYN_FIXED);
		assert(connId == 5);
		conn_id_5 = connId;
	}

	int conn_id_6;
	ConnectionGeneratorFromFile* conngen_6 = nullptr;
	{
		conngen_6 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_6_3_5.dat");
		auto connId = carlsim->connect(3, 5, conngen_6, SYN_FIXED);
		assert(connId == 6);
		conn_id_6 = connId;
	}

	int conn_id_7;
	ConnectionGeneratorFromFile* conngen_7 = nullptr;
	{
		conngen_7 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_7_3_6.dat");
		auto connId = carlsim->connect(3, 6, conngen_7, SYN_FIXED);
		assert(connId == 7);
		conn_id_7 = connId;
	}

	int conn_id_8;
	ConnectionGeneratorFromFile* conngen_8 = nullptr;
	{
		conngen_8 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_8_6_5.dat");
		auto connId = carlsim->connect(6, 5, conngen_8, SYN_FIXED);
		assert(connId == 8);
		conn_id_8 = connId;
	}

	int conn_id_9;
	ConnectionGeneratorFromFile* conngen_9 = nullptr;
	{
		conngen_9 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_9_5_7.dat");
		auto connId = carlsim->connect(5, 7, conngen_9, SYN_FIXED);
		assert(connId == 9);
		conn_id_9 = connId;
	}

	int conn_id_10;
	ConnectionGeneratorFromFile* conngen_10 = nullptr;
	{
		conngen_10 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_10_5_8.dat");
		auto connId = carlsim->connect(5, 8, conngen_10, SYN_FIXED);
		assert(connId == 10);
		conn_id_10 = connId;
	}

	int conn_id_11;
	ConnectionGeneratorFromFile* conngen_11 = nullptr;
	{
		conngen_11 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_11_8_7.dat");
		auto connId = carlsim->connect(8, 7, conngen_11, SYN_FIXED);
		assert(connId == 11);
		conn_id_11 = connId;
	}

	int conn_id_12;
	ConnectionGeneratorFromFile* conngen_12 = nullptr;
	{
		conngen_12 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_12_7_1.dat");
		auto connId = carlsim->connect(7, 1, conngen_12, SYN_FIXED);
		assert(connId == 12);
		conn_id_12 = connId;
	}

	int conn_id_13;
	ConnectionGeneratorFromFile* conngen_13 = nullptr;
	{
		conngen_13 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_13_7_2.dat");
		auto connId = carlsim->connect(7, 2, conngen_13, SYN_FIXED);
		assert(connId == 13);
		conn_id_13 = connId;
	}

	int conn_id_14;
	ConnectionGeneratorFromFile* conngen_14 = nullptr;
	{
		conngen_14 = new ConnectionGeneratorFromFile("synfire4loopB/conngrpgen_14_2_1.dat");
		auto connId = carlsim->connect(2, 1, conngen_14, SYN_FIXED);
		assert(connId == 14);
		conn_id_14 = connId;
	}

