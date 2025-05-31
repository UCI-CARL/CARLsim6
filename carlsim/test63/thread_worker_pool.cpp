#include "gtest/gtest.h"

#include <condition_variable>
#include <mutex>
#include <vector>
#include <thread>
#include <windows.h>
#include <iostream>
#include <cmath>

long long int result2 = 0;
double result3 = 1.0;

void do_calculation2(int i) {
	result2 += 1;
	result3 += result3 * 1.111 / 3.14 * (std::sqrt(result3) * (result2 * (result2 * 0.001 + 20)));
}

class Pool {
public:
	struct Worker {
		std::thread* thread;
		bool ready = false;
		bool done = false;
		int core_id = 0;
		Pool* pool = nullptr;

		void worker_thread() {
			int count = 0;
			while (true) {
				std::unique_lock<std::mutex> lock(pool->mtx_);
				pool->cv_.wait(lock, [&] { return ready || done; });

				if (done) break;

				do_calculation2(count);

				ready = false;
				++count;

				pool->cv_.notify_all();
			}
		}
	};

	Pool(int num_workers, int core_offset = 0)
		: num_workers_(num_workers)
	{
		for (int p = 0; p < num_workers_; ++p) {
			Worker* worker = new Worker;
			worker->core_id = p + core_offset;
			worker->pool = this;
			worker->thread = new std::thread(&Worker::worker_thread, worker);
			SetThreadAffinityMask(worker->thread->native_handle(), 1ull << worker->core_id);
			workers_.push_back(worker);
		}
	}

	~Pool() {
		stop();
		for (auto w : workers_) {
			delete w->thread;
			delete w;
		}
	}

	void runNext() {
		
			// 1. Set all workers ready and notify all
			{
				std::lock_guard<std::mutex> lock(mtx_);
				for (auto w : workers_) {
					w->ready = true;
				}
			}
			cv_.notify_all();

			// 2. Wait for all workers to finish
			std::unique_lock<std::mutex> lock(mtx_);
			cv_.wait(lock, [&] {
				for (auto w : workers_)
					if (w->ready) return false;
				return true;
				});
		
	}

	void stop() {
		{
			std::lock_guard<std::mutex> lock(mtx_);
			for (auto w : workers_) {
				w->done = true;
				w->ready = true;
			}
		}
		cv_.notify_all();
		for (auto w : workers_) {
			if (w->thread->joinable())
				w->thread->join();
		}
	}

	int num_workers() const { return num_workers_; }

private:
	std::vector<Worker*> workers_;
	int num_workers_;
	std::mutex mtx_;
	std::condition_variable cv_;
};

TEST(Thread, Pool) {
	int Partitions = 4;
	int N = 1000000 / Partitions;
	long long int expected = 0;
	for (int p = 0; p < Partitions; p++)
		for (int i = 0; i < N; ++i)
			expected += 1;

	{
		Pool pool(Partitions);
		for (int i = 0; i < N; ++i) {
			pool.runNext();
		}
		// Pool destructor will stop and join all threads
	}

	printf("result %llu\n", result2);
	EXPECT_EQ(result2, expected);
}