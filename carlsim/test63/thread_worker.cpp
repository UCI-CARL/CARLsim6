#include "gtest/gtest.h"

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>


std::mutex m;

std::condition_variable cv;

std::string data;

bool ready = false;
bool processed = false;
 
bool running = true; 

bool isReady() { return ready; }
bool isProcessed() { return processed; }

void worker_thread()
{
	while (running) {

		// wait until main() sends data
		std::unique_lock<std::mutex> lk(m);

		//std::cout << "Worker thread is waiting for data\n"; 

		//cv.wait(lk, &[]() { return ready; });
		cv.wait(lk, isReady);

		// after the wait, we own the lock
		std::cout << "Worker thread is processing data\n";
		data += " processed";
 
		// send data back to main()
		processed = true;
		//std::cout << "Worker thread signals data processing completed\n";
 
		// manual unlocking is done before notifying, to avoid waking up
		// the waiting thread only to block again (see notify_one for details)
		lk.unlock();

		cv.notify_one();

	}
}
 

TEST(Threads, WorkerX) {


	std::thread worker(worker_thread);

	data = "";

	for (int i = 0; i < 3 + 1; i++) {
		
		// send data to the worker thread
		{
			std::lock_guard<std::mutex> lk(m);
			ready = true;
			//std::cout << "main() signals data ready for processing\n";
		}
		cv.notify_one();

		// wait for the worker
		{
			std::unique_lock<std::mutex> lk(m);
			//cv.wait(lk, &[] { return processed; });
			cv.wait(lk, isProcessed );

		}
		//std::cout << "Back in main(), data = " << data << '\n';


	}

	std::cout << data.size() << '\n';

	running = false; 
	
	worker.join();
	
}