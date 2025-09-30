	// Instanciate the CARLsim simulation object
	CARLsim* carlsim = new CARLsim("synfireMin", (SimMode)1, (LoggerMode)0, 0, 42);

	// CONFIG STATE
	carlsim->setIntegrationMethod(RUNGE_KUTTA4, 10);

	carlsim->setupNetwork();

