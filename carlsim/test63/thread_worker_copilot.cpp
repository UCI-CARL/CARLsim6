#include "gtest/gtest.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
bool done = false;
 
int result = 0; 

// Example calculation function
void do_calculation(int i) {
	// Arbitrary calculation (replace with real work)
	//std::cout << "Calculation " << i << " done by worker.\n";
	result += 10 * i;
}

void worker_thread() {
	int count = 0;
	while (true) {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [] { return ready || done; });

		if (done) break;

		// Perform calculation
		do_calculation(count);

		ready = false; 
		++count;

		lock.unlock();
		cv.notify_one();
	}
}

TEST(Threads, Worker) {
	std::thread worker(worker_thread);

	int N = 100000; 

	int expected = 0;
	for (int i = 0; i < N; ++i) 
		expected += 10 * i;

	for (int i = 0; i < N; ++i) {
		{
			std::lock_guard<std::mutex> lock(mtx);
			ready = true;
		}
		cv.notify_one();

		// Wait for worker to finish
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [] { return !ready; });
	}

	// Signal worker to exit
	{
		std::lock_guard<std::mutex> lock(mtx);
		done = true;
		ready = true;
	}
	cv.notify_one();

	worker.join();
	//std::cout << "All calculations done.\n";

	EXPECT_EQ(result, expected);
}