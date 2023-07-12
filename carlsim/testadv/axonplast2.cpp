#ifdef LN_AXON_PLAST

#include "gtest/gtest.h"
#include "carlsim_tests.h"

#include <carlsim.h>

// include stopwatch for timing
#include <stopwatch.h>
#include <periodic_spikegen.h>
#include <algorithm>

/*
* V 2: 
* Maze traversal costs
* findWavefront can handle ... 
*
* based on Sample Maze 4x4
*/


class Maze
{
public:
	static const int rows = 4;
	static const int columns = 4;
	unsigned maze[rows][columns];

	// map is ordered ascending by key (sorted)
	// when is a tuple a > b ?   
	typedef std::map<std::tuple<int, int>, uint8_t> delay_map_t;  

	typedef std::vector<std::tuple<int, int, uint8_t>> ConnDelays_t;

	delay_map_t map;  // representing the traversal costs


	float weight = 85.f;  // current
	float maxWt = 400.0f;
	//float weight = 0.35f; // conductance
	//float maxWt = 4.0f;

	int n;
	bool backward;

	Maze(unsigned _maze[4][4], bool backward = false): backward(backward) {
		n = rows * columns;
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				maze[r][c] = _maze[r][c];
		initialize();
	}

	// this would be an environment, with uniquous cost
	Maze(unsigned cost, bool backward = false) : backward(backward) {
		n = rows * columns;
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				maze[r][c] = cost;
		initialize();
	}



	// check Grid3D of group 

	//! row, column start with 0 (not 1)
	//! provide conversion to 1_based x,y based 
	int index(int row, int column) {
		// index(r, c) = > (r * 4) + c
		return row * columns + column;
	}

	int row(int index) {
		return trunc(index / columns);
	}

	int column(int index) {
		return index % rows;
	}

	int x(int c) { return c + 1; }
	int y(int r) { return r + 1; }

	int c(int x) { return x - 1; }
	int r(int y) { return y - 1; }

	int indexXY(int x, int y) { return index(r(y), c(x)); }


	/* converts the maze into delays and Point3Ds
	*/
	void initialize() {

		int pre, post;
		uint8_t d;

		// forward column wise, starting at (0,0)
		// row and column is 0 based, use (x,y) to convert
		for (unsigned r = 0; r < rows; r++)
			for (unsigned c = 0; c < columns - 1; c++) {
				d = maze[r][c]; // traverse costs
				pre = index(r, c);
				post = index(r, c + 1);
				map.emplace(std::tuple<int, int>(pre, post), d);  
			}

		// forward row wise
		for (unsigned r = 0; r < rows - 1; r++)
			for (unsigned c = 0; c < columns; c++) {
				d = maze[r][c]; // traverse costs
				pre = index(r, c);
				post = index(r + 1, c);
				map.emplace(std::tuple<int, int>(pre, post), d);
			}

		if (backward) {
		// backward colunn wise
		for (unsigned r = 0; r < rows; r++)
			for (unsigned c = 0; c < columns - 1; c++) {
				d = maze[r][c + 1]; // traverse costs
				pre = index(r, c + 1);
				post = index(r, c);
				map.emplace(std::tuple<int, int>(pre, post), d);
			}

		// backward row wise
		for (unsigned r = 0; r < rows - 1; r++)
			for (unsigned c = 0; c < columns; c++) {
				d = maze[r + 1][c]; // traverse costs
				pre = index(r + 1, c);
				post = index(r, c);
				map.emplace(std::tuple<int, int>(pre, post), d);
			}

		}
	}


	// fix base cost for EProp
	void initializeConnDelays(ConnDelays_t& connDelays, uint8_t cost) {
		connDelays.resize(0);
		connDelays.clear();
		for (unsigned r = 0; r < rows; r++)
			for (unsigned c = 0; c < columns; c++) {
				connDelays.push_back(std::tuple<int, int, uint8_t>(r, c, cost));
			}
	}

	// optimal values
	void initializeConnDelays(ConnDelays_t& connDelays) {
		connDelays.resize(0);
		for (unsigned r = 0; r < rows; r++)
			for (unsigned c = 0; c < columns; c++) {
				connDelays.push_back(std::tuple<int, int, uint8_t>(r, c, maze[r][c]));
				//printf("connDelays[%d] = (%d, %d) --> %d\n", (int)connDelays.size() - 1, r, c, maze[r][c]);
			}
	}


};


class MazeConnGen2 : public ConnectionGenerator
{
public:

	Maze* maze;

	Maze::delay_map_t map;
	Maze::ConnDelays_t connDelays;  // pre, post, delay  (map as list, could be map iterator )

	std::vector<Point3D> neurons3D;

	enum loglevel { ERR, INF, DEB, DEV } ll;

	// Create a Connection generator on a Maze as this defines the connections.
	// However, do assign the connection in a seperate routine
	MazeConnGen2(Maze* maze) : maze(maze) {};


	// fix base cost for EProp
	void initializeConnDelays(uint8_t cost = 0) {
		connDelays.resize(0);
		connDelays.clear();
		std::for_each(maze->map.begin(), maze->map.end(), [&](auto &key_value) {  //std::pair
			auto key = std::get<0>(key_value); 
			auto value = std::get<1>(key_value);
			auto pre = std::get<0>(key);
			auto post = std::get<1>(key);
			auto d = cost > 0 ? cost : value;
			connDelays.push_back(std::tuple<int, int, uint8_t>(pre, post, d));
			map.emplace(std::tuple<int, int>(pre, post), d);
			//printf("connDelays[%2d]: %2d --> %2d = %2d\n", (int)connDelays.size() - 1, pre, post, d);
			}
		);

	}


	virtual void connect(CARLsim* s,
		int srcGrpId, int i,	// pre
		int destGrpId, int j,	// post
		float& weight,
		float& maxWt,
		float& delay,
		bool& connected) {

		weight = maze->weight;
		maxWt = maze->maxWt;

		try {
			delay = map.at(std::tuple<int, int>(i, j));  // with bounds checking [] inserts "missing" elements
			connected = true;
			//Point3D& from = neurons3D[i], & to = neurons3D[j];
			//if (ll >= DEV) printf("connect [%d](%d,%d) -> [%d](%d,%d) = %.0f \n", i, int(from.y), int(from.x), j, int(to.y), int(to.x), delay);  // row, column, Fig.1.B
		}
		catch (std::out_of_range) {
			delay = 0.f;
			connected = false;
		}
	}

};


/*
* find Wavefront Path
*/
TEST(axonplast2, path) {

	unsigned maze_ref1[4][4] = {
		//    1     4
			{ 1,1,1,1 },	// 0, 1, 2, 3,    	
			{ 1,2,2,1 },	// 4, 5, 6, 7,
			{ 1,2,2,1 },	// 8, 9, 10, 11, 
			{ 1,2,1,1 }	    // 12, 13, 14, 15
	};


	// ---------------- CONFIG STATE -------------------

	CARLsim sim("axonplast_path", CPU_MODE, USER);

	int CA3_Pyramidal = sim.createGroup("CA3_Pyramidal", 16, // 4x4
		EXCITATORY_NEURON, -1, CPU_CORES);
	sim.setNeuronParameters(CA3_Pyramidal, 0.02f, 0.2f, -65.0f, 8.0f); // RS

	//// any SNN must be damped inorder to be roboust
	//int CA3_Basket = sim.createGroup("CA3_Basket", 16, // 4x4
	//	INHIBITORY_NEURON, -1, CPU_CORES);
	//sim.setNeuronParameters(CA3_Basket, 0.1f, 0.2f, -65.0f, 2.0f); // FS

	//Maze* maze = new Maze(maze_ref1);
	Maze* maze = new Maze(maze_ref1, true);  // Show case without Interneurons
	MazeConnGen2 conn(maze);

	conn.initializeConnDelays(); // optimal values
	//conn.initializeConnDelays(4); // initial value 
	//conn.initializeConnDelays(1); // initial value 

	//MyConnGen conn;
	
	//EXPECT_EQ(conn.index(2, 3), 6);
	//EXPECT_EQ(conn.row(6), 2);
	//EXPECT_EQ(conn.column(6), 3);


	sim.connect(CA3_Pyramidal, CA3_Pyramidal, &conn, SYN_FIXED);

	//sim.connect(CA3_Pyramidal, CA3_Basket, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1)); // trigger inhib
	//sim.connect(CA3_Basket, CA3_Pyramidal, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1)); // inhib pyramidal

	// avoid warning  setConductances has not been called. Setting simulation mode to CUBA.
	sim.setConductances(false);

	// ---------------- SETUP STATE -------------------
	sim.setupNetwork();

	SpikeMonitor* spkMon = sim.setSpikeMonitor(CA3_Pyramidal, "DEFAULT");
	//spkMon->setMode(AER);
	spkMon->setPersistentData(false);

	auto printSpikes = [&]() {
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		printf(" |  1  2  3  4  |\n");
		printf("-+--------------+\n");
		for (int y = 1; y <= 4; y++) {
			printf("%d|", y);
			for (int x = 1; x <= 4; x++) {
				int i = conn.maze->indexXY(x, y);
				int t = -1;
				if (!spikes[i].empty()) {
					t = spikes[i][0];
					printf("%3d", t);
				}
				else {
					printf("   ");
				}
			}
			printf("  |\n");
		}
		printf("-+--------------+\n");
	};

	//check if target neuron has fired at current time
	auto checkTargetFired = [&](int targetNId, unsigned int currentTime) {
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		if (!spikes[targetNId].empty()) {
			auto& firings = spikes[targetNId];
			auto fired = firings.back();
			return currentTime - fired <= 1;  // SpikeMon has 1ms latency
		}
		return false;
	};

	// ---------------- RUN STATE -------------------
	//sim.setExternalCurrent(CA3_Pyramidal, 100.0);  //   wave prop
	std::vector<float> current(16, 0.f);

	current[0] = 100.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);
	sim.runNetwork(0, 1);  // longest path 2x4
	current[0] = 0.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);


	// ------------------------------
	int numPreN, numPostN;
	uint8_t* delays = sim.getDelays(CA3_Pyramidal, CA3_Pyramidal, numPreN, numPostN);

	printf("  \\ post\n");
	printf("pre");
	for (int j = 0; j < numPostN; j++)
		printf(" %02d", j);
	printf("\n");

	// 2 x 12 of 16x(16-1)
	for (int i = 0; i < numPreN; i++) {
		printf(" %02d", i);
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			//int d = delays[j + i * numPreN];
			if (d > 0) {
				// if DEBUG_AXONPLAST
				printf(" % 2d", d);
			}
			else
				printf(i == j ? "  *": "   ");
		}
		printf("\n");
	}

	std::vector<std::tuple<int, int, uint8_t>> connDelays;  // pre, post, delay
	EXPECT_EQ(numPreN, numPostN);
	for (int i = 0; i < numPreN; i++) {
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			//int row_i = maze->row(i);
			//int column_i = maze->column(i);
			//int row_j = maze->row(j);
			//int column_j = maze->column(j);
			int y_i = maze->y(maze->row(i));
			int x_i = maze->x(maze->column(i));
			int y_j = maze->y(maze->row(j));
			int x_j = maze->x(maze->column(j));
			if (d > 0) {
				// printf("[%d](%d,%d)->[%d](%d,%d): %d\n", i, y_i, x_i, j, y_j, x_j, d);
			}

		}
	}
	delete delays;

	// ------------------


	sim.saveSimulation("axonplast2__path__01_initial.dat", true); // fileName, saveSynapseInfo

	auto startNid = 0;
	auto targetNid = 15;
	auto targetFired = false;
	auto timeOut = 30; 
		// optimal path (d_opt=1): t = 1 + (7-1) * 1  --> 7 
		// init d=4:  t = 1 + (7 - 1) * 4   --> 25
	for (auto t = 1; !targetFired && t <= timeOut; t++) {  
		spkMon->startRecording();
		sim.runNetwork(0, 1, true);  // longest path 2x4
		spkMon->stopRecording();
		printSpikes();
		targetFired = checkTargetFired(targetNid, sim.getSimTime());
		spkMon->clear();
	}

	if(!targetFired) {
		printf("Target not reached at %d, breaking up.\n", sim.getSimTime());
		return;
	}

	std::vector<int> path;
	std::vector<float> eligibility (16, 0.0f);
	sim.findWavefrontPath(path, eligibility, 8, CA3_Pyramidal, 0, 15);  // Design flaw,  grpIds are global, see getDelays,  netId must not be passed but derived

	const int buffer_len = 100;
	char buffer[buffer_len];
	const char* filename = "eligibility.txt";
	std::FILE* file = std::fopen(filename, "w");
	for (int nId = 0; nId < 16; nId++) {
		sprintf(buffer, "%d %.3f\n", nId, eligibility[nId]);
		fputs(buffer, file);
	}
	std::fclose(file);


	auto print_D = [&]() {
		for (auto iter = connDelays.begin(); iter != connDelays.end(); iter++) {
			int pre = std::get<0>(*iter);
			int post = std::get<1>(*iter);
			uint8_t d = std::get<2>(*iter);
			printf("D_%d_%d = %d\n", pre, post, int(d));
		}
	};

	print_D();


	sim.saveSimulation("axonplast2__path__02_findWavefrontPath.dat", true); // fileName, saveSynapseInfo
}



/*
* find Wavefront Path
*/
TEST(axonplast2, interneurons) {

	unsigned maze_ref1[4][4] = {
		//    1     4
			{ 1,1,1,1 },	// 0, 1, 2, 3,    	
			{ 1,2,2,1 },	// 4, 5, 6, 7,
			{ 1,2,2,1 },	// 8, 9, 10, 11, 
			{ 1,2,1,1 }	    // 12, 13, 14, 15
	};


	// ---------------- CONFIG STATE -------------------

	CARLsim sim("axonplast_path", CPU_MODE, USER);

	int CA3_Pyramidal = sim.createGroup("CA3_Pyramidal", 16, // 4x4
		EXCITATORY_NEURON, -1, CPU_CORES);
	sim.setNeuronParameters(CA3_Pyramidal, 0.02f, 0.2f, -65.0f, 8.0f); // RS

																	   
	// any SNN must be damped inorder to be roboust
	int CA3_Basket = sim.createGroup("CA3_Basket", 16, // 4x4
		INHIBITORY_NEURON, -1, CPU_CORES);
	sim.setNeuronParameters(CA3_Basket, 0.1f, 0.2f, -65.0f, 2.0f); // FS


	Maze* maze = new Maze(maze_ref1, true);
	MazeConnGen2 conn(maze);

	conn.initializeConnDelays(); // optimal values
	//conn.initializeConnDelays(4); // initial value 
	//conn.initializeConnDelays(1); // initial value 

	//MyConnGen conn;

	//EXPECT_EQ(conn.index(2, 3), 6);
	//EXPECT_EQ(conn.row(6), 2);
	//EXPECT_EQ(conn.column(6), 3);

	sim.connect(CA3_Pyramidal, CA3_Pyramidal, &conn, SYN_FIXED);  // SYN_AXONPLAST

	sim.connect(CA3_Pyramidal, CA3_Basket, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1), SYN_FIXED); // trigger inhib
	sim.connect(CA3_Basket, CA3_Pyramidal, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1), SYN_FIXED); // inhib pyramidal


	// avoid warning  setConductances has not been called. Setting simulation mode to CUBA.
	sim.setConductances(false);

	// ---------------- SETUP STATE -------------------
	sim.setupNetwork();

	SpikeMonitor* spkMon = sim.setSpikeMonitor(CA3_Pyramidal, "DEFAULT");
	//spkMon->setMode(AER);
	spkMon->setPersistentData(false);

	auto printSpikes = [&]() {
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		printf(" |  1  2  3  4  |\n");
		printf("-+--------------+\n");
		for (int y = 1; y <= 4; y++) {
			printf("%d|", y);
			for (int x = 1; x <= 4; x++) {
				int i = conn.maze->indexXY(x, y);
				int t = -1;
				if (!spikes[i].empty()) {
					t = spikes[i][0];
					printf("%3d", t);
				}
				else {
					printf("   ");
				}
			}
			printf("  |\n");
		}
		printf("-+--------------+\n");
	};

	//check if target neuron has fired at current time
	auto checkTargetFired = [&](int targetNId, unsigned int currentTime) {
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		if (!spikes[targetNId].empty()) {
			auto& firings = spikes[targetNId];
			auto fired = firings.back();
			return currentTime - fired <= 1;  // SpikeMon has 1ms latency
		}
		return false;
	};

	// ---------------- RUN STATE -------------------
	//sim.setExternalCurrent(CA3_Pyramidal, 100.0);  //   wave prop
	std::vector<float> current(16, 0.f);

	current[0] = 100.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);
	sim.runNetwork(0, 1);  // longest path 2x4
	current[0] = 0.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);


	// ------------------------------
	int numPreN, numPostN;
	uint8_t* delays = sim.getDelays(CA3_Pyramidal, CA3_Pyramidal, numPreN, numPostN);

	printf("  \\ post\n");
	printf("pre");
	for (int j = 0; j < numPostN; j++)
		printf(" %02d", j);
	printf("\n");

	// 2 x 12 of 16x(16-1)
	for (int i = 0; i < numPreN; i++) {
		printf(" %02d", i);
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			//int d = delays[j + i * numPreN];
			if (d > 0) {
				// TODO if DEBUG_AXONPLAST
				printf(" % 2d", d);
			}
			else
				printf(i == j ? "  *" : "   ");
		}
		printf("\n");
	}

	std::vector<std::tuple<int, int, uint8_t>> connDelays;  // pre, post, delay
	EXPECT_EQ(numPreN, numPostN);
	for (int i = 0; i < numPreN; i++) {
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			//int row_i = maze->row(i);
			//int column_i = maze->column(i);
			//int row_j = maze->row(j);
			//int column_j = maze->column(j);
			int y_i = maze->y(maze->row(i));
			int x_i = maze->x(maze->column(i));
			int y_j = maze->y(maze->row(j));
			int x_j = maze->x(maze->column(j));
			if (d > 0) {
				//printf("[%d](%d,%d)->[%d](%d,%d): %d\n", i, row_i, column_i, j, row_j, column_j, d);
				printf("[%d](%d,%d)->[%d](%d,%d): %d\n", i, y_i, x_i, j, y_j, x_j, d);
				//connDelays.push_back(std::tuple<int, int, uint8_t>(i, j, d));
			}

		}
	}
	delete delays;

	// ------------------


	sim.saveSimulation("axonplast2__path__01_initial.dat", true); // fileName, saveSynapseInfo

	auto startNid = 0;
	auto targetNid = 15;
	auto targetFired = false;
	auto timeOut = 30;
	// optimal path (d_opt=1): t = 1 + (7-1) * 1  --> 7 
	// init d=4:  t = 1 + (7 - 1) * 4   --> 25
	for (auto t = 1; !targetFired && t <= timeOut; t++) {
		spkMon->startRecording();
		sim.runNetwork(0, 1, true);  // longest path 2x4
		spkMon->stopRecording();
		printSpikes();
		targetFired = checkTargetFired(targetNid, sim.getSimTime());
		spkMon->clear();
	}

	if (!targetFired) {
		printf("Target not reached at %d, breaking up.\n", sim.getSimTime());
		return;
	}

	std::vector<int> path;
	std::vector<float> eligibility (16, 0.0f);
	sim.findWavefrontPath(path, eligibility, 8, CA3_Pyramidal, 0, 15);  // Design flaw,  grpIds are global, see getDelays,  netId must not be passed but derived

	const int buffer_len = 100;
	char buffer[buffer_len];
	const char* filename = "eligibility.txt";
	std::FILE* file = std::fopen(filename, "w");
	for (int nId = 0; nId < 16; nId++) {
		sprintf(buffer, "%d %.3f\n", nId, eligibility[nId]);
		fputs(buffer, file);
	}
	std::fclose(file);


	auto print_D = [&]() {
		for (auto iter = connDelays.begin(); iter != connDelays.end(); iter++) {
			int pre = std::get<0>(*iter);
			int post = std::get<1>(*iter);
			uint8_t d = std::get<2>(*iter);
			printf("D_%d_%d = %d\n", pre, post, int(d));
		}
	};

	print_D();


	sim.saveSimulation("axonplast2__path__02_findWavefrontPath.dat", true); // fileName, saveSynapseInfo
}




/*
* find Wavefront Path
*/
TEST(axonplast2, conductance) {

	unsigned maze_ref1[4][4] = {
		//    1     4
			{ 1,1,1,1 },	// 0, 1, 2, 3,    	
			{ 1,2,2,1 },	// 4, 5, 6, 7,
			{ 1,2,2,1 },	// 8, 9, 10, 11, 
			{ 1,2,1,1 }	    // 12, 13, 14, 15
	};


	// ---------------- CONFIG STATE -------------------

	CARLsim sim("axonplast_path", CPU_MODE, USER);

	int CA3_Pyramidal = sim.createGroup("CA3_Pyramidal", 16, // 4x4
		EXCITATORY_NEURON, -1, CPU_CORES);
	sim.setNeuronParameters(CA3_Pyramidal, 0.02f, 0.2f, -65.0f, 8.0f); // RS


	// any SNN must be damped inorder to be roboust
	int CA3_Basket = sim.createGroup("CA3_Basket", 16, // 4x4
		INHIBITORY_NEURON, -1, CPU_CORES);
	sim.setNeuronParameters(CA3_Basket, 0.02, 0.2f, -50.0f, 2.75f); // xx


	Maze* maze = new Maze(maze_ref1, true);
	// conductance
	maze->weight = 0.35f; 
	maze->maxWt = 4.0f;

	MazeConnGen2 conn(maze);

	conn.initializeConnDelays(); // optimal values
	//conn.initializeConnDelays(4); // initial value 
	//conn.initializeConnDelays(1); // initial value 

	//MyConnGen conn;

	//EXPECT_EQ(conn.index(2, 3), 6);
	//EXPECT_EQ(conn.row(6), 2);
	//EXPECT_EQ(conn.column(6), 3);

	sim.connect(CA3_Pyramidal, CA3_Pyramidal, &conn, SYN_FIXED);  // SYN_AXONPLAST

	sim.connect(CA3_Pyramidal, CA3_Basket, "one-to-one", RangeWeight(0.75), 1.0f, RangeDelay(1), SYN_FIXED); // trigger inhib
	sim.connect(CA3_Basket, CA3_Pyramidal, "one-to-one", RangeWeight(3.5), 1.0f, RangeDelay(1), SYN_FIXED); // inhib pyramidal


	// avoid warning  setConductances has not been called. Setting simulation mode to CUBA.
	//sim.setConductances(false);
	sim.setConductances(true, 2, 15, 3, 15);

	// ---------------- SETUP STATE -------------------
	sim.setupNetwork();

	SpikeMonitor* spkMon = sim.setSpikeMonitor(CA3_Pyramidal, "DEFAULT");
	//spkMon->setMode(AER);
	spkMon->setPersistentData(false);

	auto printSpikes = [&]() {
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		printf(" |  1  2  3  4  |\n");
		printf("-+--------------+\n");
		for (int y = 1; y <= 4; y++) {
			printf("%d|", y);
			for (int x = 1; x <= 4; x++) {
				int i = conn.maze->indexXY(x, y);
				int t = -1;
				if (!spikes[i].empty()) {
					t = spikes[i][0];
					printf("%3d", t);
				}
				else {
					printf("   ");
				}
			}
			printf("  |\n");
		}
		printf("-+--------------+\n");
	};

	//check if target neuron has fired at current time
	auto checkTargetFired = [&](int targetNId, unsigned int currentTime) {
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		if (!spikes[targetNId].empty()) {
			auto& firings = spikes[targetNId];
			auto fired = firings.back();
			return currentTime - fired <= 1;  // SpikeMon has 1ms latency
		}
		return false;
	};

	// ---------------- RUN STATE -------------------
	//sim.setExternalCurrent(CA3_Pyramidal, 100.0);  //   wave prop
	std::vector<float> current(16, 0.f);

	current[0] = 100.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);
	sim.runNetwork(0, 1);  // longest path 2x4
	current[0] = 0.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);


	// ------------------------------
	int numPreN, numPostN;
	uint8_t* delays = sim.getDelays(CA3_Pyramidal, CA3_Pyramidal, numPreN, numPostN);

	printf("  \\ post\n");
	printf("pre");
	for (int j = 0; j < numPostN; j++)
		printf(" %02d", j);
	printf("\n");

	// 2 x 12 of 16x(16-1)
	for (int i = 0; i < numPreN; i++) {
		printf(" %02d", i);
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			//int d = delays[j + i * numPreN];
			if (d > 0) {
				// TODO if DEBUG_AXONPLAST
				printf(" % 2d", d);
			}
			else
				printf(i == j ? "  *" : "   ");
		}
		printf("\n");
	}

	std::vector<std::tuple<int, int, uint8_t>> connDelays;  // pre, post, delay
	EXPECT_EQ(numPreN, numPostN);
	for (int i = 0; i < numPreN; i++) {
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			//int row_i = maze->row(i);
			//int column_i = maze->column(i);
			//int row_j = maze->row(j);
			//int column_j = maze->column(j);
			int y_i = maze->y(maze->row(i));
			int x_i = maze->x(maze->column(i));
			int y_j = maze->y(maze->row(j));
			int x_j = maze->x(maze->column(j));
			if (d > 0) {
				//printf("[%d](%d,%d)->[%d](%d,%d): %d\n", i, row_i, column_i, j, row_j, column_j, d);
				printf("[%d](%d,%d)->[%d](%d,%d): %d\n", i, y_i, x_i, j, y_j, x_j, d);
				//connDelays.push_back(std::tuple<int, int, uint8_t>(i, j, d));
			}

		}
	}
	delete delays;

	// ------------------


	sim.saveSimulation("axonplast2__path__01_initial.dat", true); // fileName, saveSynapseInfo

	auto startNid = 0;
	auto targetNid = 15;
	auto targetFired = false;
	auto timeOut = 30;
	// optimal path (d_opt=1): t = 1 + (7-1) * 1  --> 7 
	// init d=4:  t = 1 + (7 - 1) * 4   --> 25
	for (auto t = 1; !targetFired && t <= timeOut; t++) {
		spkMon->startRecording();
		sim.runNetwork(0, 1, true);  // longest path 2x4
		spkMon->stopRecording();
		printSpikes();
		targetFired = checkTargetFired(targetNid, sim.getSimTime());
		spkMon->clear();
	}

	if (!targetFired) {
		printf("Target not reached at %d, breaking up.\n", sim.getSimTime());
		return;
	}

	std::vector<int> path;
	std::vector<float> eligibility (16, 0.0f);
	sim.findWavefrontPath(path, eligibility, 8, CA3_Pyramidal, 0, 15);  

	const int buffer_len = 100;
	char buffer[buffer_len];
	const char* filename = "eligibility.txt";
	std::FILE* file = std::fopen(filename, "w");
	for (int nId = 0; nId < 16; nId++) {
		sprintf(buffer, "%d %.3f\n", nId, eligibility[nId]);
		fputs(buffer, file);
	}
	std::fclose(file);


	auto print_D = [&]() {
		for (auto iter = connDelays.begin(); iter != connDelays.end(); iter++) {
			int pre = std::get<0>(*iter);
			int post = std::get<1>(*iter);
			uint8_t d = std::get<2>(*iter);
			printf("D_%d_%d = %d\n", pre, post, int(d));
		}
	};

	print_D();


	sim.saveSimulation("axonplast2__path__02_findWavefrontPath.dat", true); // fileName, saveSynapseInfo
}




/*
* find Wavefront Path
*/
TEST(axonplast2, grouplevel) {

	unsigned maze_ref1[4][4] = {
		//    1     4
			{ 1,1,1,1 },	// 0, 1, 2, 3,    	
			{ 1,2,2,1 },	// 4, 5, 6, 7,
			{ 1,2,2,1 },	// 8, 9, 10, 11, 
			{ 1,2,1,1 }	    // 12, 13, 14, 15
	};

	// ---------------- CONFIG STATE -------------------

	CARLsim sim("axonplast_path", CPU_MODE, USER);

	int CA3_Pyramidal = sim.createGroup("CA3_Pyramidal", 16, // 4x4
		EXCITATORY_NEURON, -1, CPU_CORES);
	sim.setNeuronParameters(CA3_Pyramidal, 0.02f, 0.2f, -65.0f, 8.0f); // RS

	// any SNN must be damped inorder to be roboust
	int CA3_Basket = sim.createGroup("CA3_Basket", 16, // 4x4
		INHIBITORY_NEURON, -1, CPU_CORES);
	sim.setNeuronParameters(CA3_Basket, 0.02, 0.2f, -50.0f, 2.75f); // xx


	Maze* maze = new Maze(maze_ref1, true); // forwards & backwards
	MazeConnGen2 conn(maze);

	conn.initializeConnDelays(); // optimal values
	//conn.initializeConnDelays(4); // initial value 
	//conn.initializeConnDelays(1); // initial value 

	sim.connect(CA3_Pyramidal, CA3_Pyramidal, &conn, SYN_FIXED);  // SYN_AXONPLAST

	sim.connect(CA3_Pyramidal, CA3_Basket, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1), SYN_FIXED); // trigger inhib
	sim.connect(CA3_Basket, CA3_Pyramidal, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1), SYN_FIXED); // inhib pyramidal

	sim.setConductances(CA3_Pyramidal, false);
	sim.setConductances(CA3_Basket, true, 2, 15, 3, 15);


	// ---------------- SETUP STATE -------------------

	sim.setupNetwork();

	SpikeMonitor* spkMon = sim.setSpikeMonitor(CA3_Pyramidal, "DEFAULT");
	spkMon->setPersistentData(false);

	auto printSpikes = [&]() {
		//Do not print empty spike tables
		if(spkMon->getPopNumSpikes() < 1)
			return;
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		printf(" |  1  2  3  4  |\n");
		printf("-+--------------+\n");
		for (int y = 1; y <= 4; y++) {
			printf("%d|", y);
			for (int x = 1; x <= 4; x++) {
				int i = conn.maze->indexXY(x, y);
				int t = -1;
				if (!spikes[i].empty()) {
					t = spikes[i][0];
					printf("%3d", t);
				}
				else {
					printf("   ");
				}
			}
			printf("  |\n");
		}
		printf("-+--------------+\n");
	};

	//Check if target neuron has fired at current time
	auto checkTargetFired = [&](int targetNId, unsigned int currentTime) {
		std::vector<std::vector<int>> spikes = spkMon->getSpikeVector2D();
		if (!spikes[targetNId].empty()) {
			auto& firings = spikes[targetNId];
			auto fired = firings.back();
			return currentTime - fired <= 1;  // SpikeMon has 1ms latency
		}
		return false;
	};

	// ---------------- RUN STATE -------------------


	// ------------------------------
	int numPreN, numPostN;
	uint8_t* delays = sim.getDelays(CA3_Pyramidal, CA3_Pyramidal, numPreN, numPostN);

	printf("  \\ post\n");
	printf("pre");
	for (int j = 0; j < numPostN; j++)
		printf(" %02d", j);
	printf("\n");

	// 2 x 12 of 16x(16-1)
	for (int i = 0; i < numPreN; i++) {
		printf(" %02d", i);
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			//int d = delays[j + i * numPreN];
			if (d > 0) {
				printf(" % 2d", d);
			}
			else
				printf(i == j ? "  *" : "   ");
		}
		printf("\n");
	}

	std::vector<std::tuple<int, int, uint8_t>> connDelays;  // pre, post, delay
	EXPECT_EQ(numPreN, numPostN);
	for (int i = 0; i < numPreN; i++) {
		for (int j = 0; j < numPostN; j++) {
			int d = delays[j * numPostN + i];
			int y_i = maze->y(maze->row(i));
			int x_i = maze->x(maze->column(i));
			int y_j = maze->y(maze->row(j));
			int x_j = maze->x(maze->column(j));
			if (d > 0) {
				//printf("[%d](%d,%d)->[%d](%d,%d): %d\n", i, y_i, x_i, j, y_j, x_j, d);
			}
		}
	}
	delete delays;

	// ------------------

	std::vector<float> current(16, 0.f);
	current[0] = 100.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);
	sim.runNetwork(0, 1);  // longest path 2x4
	current[0] = 0.f;
	sim.setExternalCurrent(CA3_Pyramidal, current);

	sim.saveSimulation("axonplast2__path__01_initial.dat", true); // fileName, saveSynapseInfo

	auto startNid = 0;
	auto targetNid = 15;
	auto targetFired = false;
	auto timeOut = 30;
	// optimal path (d_opt=1): t = 1 + (7-1) * 1  --> 7 
	// init d=4:  t = 1 + (7 - 1) * 4   --> 25

	for (auto t = 1; !targetFired && t <= timeOut; t++) {
		spkMon->startRecording();
		sim.runNetwork(0, 1, true);  // longest path 2x4
		spkMon->stopRecording();
		printSpikes();
		targetFired = checkTargetFired(targetNid, sim.getSimTime());
		spkMon->clear();
	}

	if (!targetFired) {
		printf("Target not reached at %d, breaking up.\n", sim.getSimTime());
		return;
	}

	std::vector<int> path;
	std::vector<float> eligibility (16, 0.0f);
	sim.findWavefrontPath(path, eligibility, 8, CA3_Pyramidal, 0, 15);  // Design flaw,  grpIds are global, see getDelays,  netId must not be passed but derived

	const int buffer_len = 100;
	char buffer[buffer_len];
	const char* filename = "eligibility.txt";
	std::FILE* file = std::fopen(filename, "w");
	for (int nId = 0; nId < 16; nId++) {
		sprintf(buffer, "%d %.3f\n", nId, eligibility[nId]);
		fputs(buffer, file);
	}
	std::fclose(file);


	auto print_D = [&]() {
		for (auto iter = connDelays.begin(); iter != connDelays.end(); iter++) {
			int pre = std::get<0>(*iter);
			int post = std::get<1>(*iter);
			uint8_t d = std::get<2>(*iter);
			printf("D_%d_%d = %d\n", pre, post, int(d));
		}
	};

	print_D();

	sim.saveSimulation("axonplast2__path__02_findWavefrontPath.dat", true); // fileName, saveSynapseInfo
}


// recurrent connection matrix (pre = post)
struct ConnectionMatrix {

	int numPreN, numPostN;
	uint8_t* delays;

	ConnectionMatrix(int numPreN = 0, int numPostN = 0) : numPreN(numPreN), numPostN(numPostN) {
		if (numPreN == 0)
			delays = nullptr;
		else {
			delays = new uint8_t[numPreN * numPostN];
			//delays = (uint8_t*) malloc(sizeof(uint8_t) * numPreN * numPostN);
			memset(delays, 0, sizeof(uint8_t) * numPreN * numPostN);
		}
	}

	ConnectionMatrix(CARLsim* sim, int gGrpId) {
		delays = sim->getDelays(gGrpId, gGrpId, numPreN, numPostN);
	}

	// copy constructor
	// https://en.cppreference.com/w/cpp/language/copy_constructor
	ConnectionMatrix(const ConnectionMatrix& other) {
		numPreN = other.numPreN;
		numPostN = other.numPostN;
		if (numPreN == 0)
			delays = nullptr;
		else {
			delays = new uint8_t[numPreN * numPostN];
			//std::copy<uint8_t, uint8_t>(other.delays, numPreN*numPostN, delays); // requires it
			//for (int i = 0; i < numPreN * numPostN; i++)
			//	delays[i] = other.delays[i];
			memcpy(delays, other.delays, sizeof(uint8_t) * numPreN * numPostN);
		}
	}

	~ConnectionMatrix() {
		if(delays)
			delete [] delays;
	};


	unsigned getDelay(int pre, int post) {
		return delays[post * numPostN + pre];
	}

	void setDelay(int pre, int post, unsigned delay) {
		delays[post * numPostN + pre] = (uint8_t)delay;
	}

	void printMatrix(int max = 50) {
		if (numPreN > max || numPostN > max)
			return;

		printf("  \\ post\n");
		printf("pre");
		for (int j = 0; j < numPostN; j++)
			printf(" %02d", j);
		printf("\n");
		for (int pre = 0; pre < numPreN; pre++) {
			printf(" %02d", pre);
			for (int post = 0; post < numPostN; post++) {
				auto d = getDelay(pre, post);
				if (d > 0) {
					printf(" %2d", (int)d);
				}
				else
					printf("   ");
			}
			printf("\n");
		}
	}

	unsigned operator()(int pre, int post) {
		return getDelay(pre, post);
	}

	bool operator == (const ConnectionMatrix& other) {
		//Compare dimensions
		if (numPreN != other.numPreN || numPostN != other.numPostN)
			return false; 

		//Compare content
		for (int i = 0; i < numPreN * numPostN; i++)
			if (delays[i] != other.delays[i])
				return false;

		return true;
	}
	
	bool operator != (const ConnectionMatrix& other) {		
		return !(*this == other);
	}
	//int diff() {}   x, y, -> left value, right value 
};


TEST(axonplast2, connectionMatrix) {

	int numPreN = 16, numPostN = 16;

	ConnectionMatrix empty;
	ConnectionMatrix before(numPreN, numPostN); 

	EXPECT_TRUE(before == before);
	EXPECT_TRUE(empty == empty);
	EXPECT_FALSE(before == empty);
	EXPECT_TRUE(before != empty);

	//ConnectionMatrix after = (const ConnectionMatrix&) before; // use copy instead move
	ConnectionMatrix after = before; // copy constructor
	EXPECT_TRUE(after == before);

	after.setDelay(3, 4, 15); 
	EXPECT_EQ(after(3, 4), 15);
	EXPECT_EQ(before(3, 4), 0);

	EXPECT_TRUE(after != before);

	after.printMatrix(); // string stream buffer, default::stdout... 


};


/*
* find Wavefront Path
*/
TEST(axonplast2, updateDelays) {

	unsigned maze_ref1[4][4] = {
		//    1     4
			{ 1,1,1,1 },	// 0, 1, 2, 3,    	
			{ 1,2,2,1 },	// 4, 5, 6, 7,
			{ 1,2,2,1 },	// 8, 9, 10, 11, 
			{ 1,2,1,1 }	    // 12, 13, 14, 15
	};

	// ---------------- CONFIG STATE -------------------

	CARLsim* sim = nullptr; 
	int CA3_Pyramidal;

	auto createSim = [&](int mode) {
		sim = new CARLsim("update_delays", mode ? GPU_MODE : CPU_MODE, SILENT);

		CA3_Pyramidal = sim->createGroup("CA3_Pyramidal", 16, // 4x4
			EXCITATORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
		sim->setNeuronParameters(CA3_Pyramidal, 0.02f, 0.2f, -65.0f, 8.0f); // RS

		// any SNN must be damped inorder to be roboust
		int CA3_Basket = sim->createGroup("CA3_Basket", 16, // 4x4
			INHIBITORY_NEURON, -1, mode ? GPU_CORES : CPU_CORES);
		sim->setNeuronParameters(CA3_Basket, 0.02, 0.2f, -50.0f, 2.75f); // xx


		Maze* maze = new Maze(maze_ref1, true); // forwards & backwards
		//Maze* maze = new Maze(maze_ref1, false); // forwards & backwards
		MazeConnGen2 conn(maze);

		//conn.initializeConnDelays(); // optimal values
		conn.initializeConnDelays(4); // initial value 
		//conn.initializeConnDelays(1); // initial value 

		sim->connect(CA3_Pyramidal, CA3_Pyramidal, &conn, SYN_FIXED);  // SYN_AXONPLAST

		sim->connect(CA3_Pyramidal, CA3_Basket, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1), SYN_FIXED); // trigger inhib
		sim->connect(CA3_Basket, CA3_Pyramidal, "one-to-one", RangeWeight(200), 1.0f, RangeDelay(1), SYN_FIXED); // inhib pyramidal

		sim->setConductances(CA3_Pyramidal, false);
		sim->setConductances(CA3_Basket, true, 2, 15, 3, 15);


		// ---------------- SETUP STATE -------------------

		sim->setupNetwork();

	};

	//Get Connection Matrix

	for (int mode = 0; mode < TESTED_MODES; mode++) {

		createSim(mode);
		ConnectionMatrix before(sim, CA3_Pyramidal);
		before.printMatrix();
		delete sim;

		// decement each value by 1 and compare the result
		int passed = 0, failed = 0;
		for (int pre = 0; pre < before.numPreN; pre++) {
			for (int post = 0; post < before.numPostN; post++) {
				auto d = before(pre, post);
				if (d > 1) {
					createSim(mode);

					auto d_expected = d - 1;


					ConnectionMatrix expected(before); // copy 
					expected.setDelay(pre, post, d_expected);

					//Maze::ConnDelays_t connDelays2;
					std::vector<std::tuple<int, int, uint8_t>> connDelays2;
					connDelays2.push_back(std::tuple<int, int, uint8_t>(pre, post, d_expected));

					printf("updateDelays pre %d  post %d  delay %d\n", pre, post, d_expected);

					sim->updateDelays(CA3_Pyramidal, CA3_Pyramidal, connDelays2);

					ConnectionMatrix after(sim, CA3_Pyramidal);

					//EXPECT_TRUE(after == expected); 
					if (after == expected) {
						passed++;
					}
					else {
						printf("\nExpected:\n");
						expected.printMatrix();
						printf("\nAfter:\n");
						after.printMatrix();
						failed++;
					}

					delete sim;
				}
			}
		}

		EXPECT_GT(passed, 0);
		EXPECT_EQ(failed, 0);
	}
}

#endif