	int conn_id_0;
	{
		auto connId = carlsim->connect(0, 1, "random", RangeWeight(1.000000), 0.260125, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 0);
		conn_id_0 = connId;
	}

	int conn_id_1;
	{
		auto connId = carlsim->connect(0, 2, "random", RangeWeight(3.500000), 0.259800, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 1);
		conn_id_1 = connId;
	}

	int conn_id_2;
	{
		auto connId = carlsim->connect(2, 1, "random", RangeWeight(2.000000), 0.394900, RangeDelay(8, 8), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 2);
		conn_id_2 = connId;
	}

	int conn_id_3;
	{
		auto connId = carlsim->connect(1, 3, "random", RangeWeight(1.000000), 0.259075, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 3);
		conn_id_3 = connId;
	}

	int conn_id_4;
	{
		auto connId = carlsim->connect(1, 4, "random", RangeWeight(3.500000), 0.256700, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 4);
		conn_id_4 = connId;
	}

	int conn_id_5;
	{
		auto connId = carlsim->connect(4, 3, "random", RangeWeight(2.000000), 0.397500, RangeDelay(8, 8), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 5);
		conn_id_5 = connId;
	}

	int conn_id_6;
	{
		auto connId = carlsim->connect(3, 5, "random", RangeWeight(1.000000), 0.260225, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 6);
		conn_id_6 = connId;
	}

	int conn_id_7;
	{
		auto connId = carlsim->connect(3, 6, "random", RangeWeight(3.500000), 0.261400, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 7);
		conn_id_7 = connId;
	}

	int conn_id_8;
	{
		auto connId = carlsim->connect(6, 5, "random", RangeWeight(2.000000), 0.398100, RangeDelay(8, 8), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 8);
		conn_id_8 = connId;
	}

	int conn_id_9;
	{
		auto connId = carlsim->connect(5, 7, "random", RangeWeight(1.000000), 0.261625, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 9);
		conn_id_9 = connId;
	}

	int conn_id_10;
	{
		auto connId = carlsim->connect(5, 8, "random", RangeWeight(3.500000), 0.258600, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 10);
		conn_id_10 = connId;
	}

	int conn_id_11;
	{
		auto connId = carlsim->connect(8, 7, "random", RangeWeight(2.000000), 0.398900, RangeDelay(8, 8), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 11);
		conn_id_11 = connId;
	}

	int conn_id_12;
	{
		auto connId = carlsim->connect(7, 1, "random", RangeWeight(1.000000), 0.259575, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 12);
		conn_id_12 = connId;
	}

	int conn_id_13;
	{
		auto connId = carlsim->connect(7, 2, "random", RangeWeight(3.500000), 0.260900, RangeDelay(10, 10), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 13);
		conn_id_13 = connId;
	}

	int conn_id_14;
	{
		auto connId = carlsim->connect(2, 1, "random", RangeWeight(2.000000), 0.392500, RangeDelay(8, 8), RadiusRF(-1.0), SYN_FIXED);
		assert(connId == 14);
		conn_id_14 = connId;
	}

