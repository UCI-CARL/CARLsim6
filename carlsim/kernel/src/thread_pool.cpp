
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ostream>
#include <thread>

#ifdef WIN32
#include <windows.h>
#endif


#include <snn.h>
#include <snn_datastructures.h>

#include <thread_pool.h>


//// Thread Helper ..  int i param 
//void do_calculation2(int i) {
//	//result2 += 1;
//	//result3 += result3 * 1.111 / 3.14 * (std::sqrt(result3) * (result2 * (result2 * 0.001 + 20)));
//
//	printf("Update SNN netId %d \n", i); 
//
//}


struct ThreadPool::Worker {
	std::thread* thread;
	bool ready = false;
	bool done = false;
	int core_id = 0;
	ThreadPool* pool = nullptr;
	void worker_thread();
	ThreadStruct args;
	std::function<void(SNN*, int netId)> snn_method;
	//void *()(int netId);  // function pointer for the shity helper static 
	// pass this as an lambda expression.. which SNN*, int as argument  ((SNN*)args.snn_pointer)->globalStateUpdate_CPU(args.netId); 
	// void [](SNN* snn_pointer, int netId) { snn_pointer->globalStateUpdate_CPU(netId); }
};



void ThreadPool::Worker::worker_thread() {
	int count = 0;
	while (true) {
		std::unique_lock<std::mutex> lock(pool->mtx_);
		pool->cv_.wait(lock, [&] { return ready || done; });

		if (done) break;

		//do_calculation2(count);  // here the helper comes in 
		//do_calculation2(args.netId);
		// count is replaced by the ThreadStruct 

		// pthread_create(&threads[threadCount], &attr, &SNN::helperUpdateTimingTable_CPU, (void*)&argsThreadRoutine[threadCount])

		//((SNN*)args.snn_pointer)->globalStateUpdate_CPU(args.netId);

//printf("Lambda %d\n", args.netId);
		snn_method((SNN*) args.snn_pointer, args.netId);

/*

#ifndef __NO_PTHREADS__ // POSIX
	// Static multithreading subroutine method - helper for the above method
	void* SNN::helperGlobalStateUpdate_CPU(void* arguments) {
		ThreadStruct* args = (ThreadStruct*) arguments;
		////printf("\nThread ID: %lu and CPU: %d\n",pthread_self(), sched_getcpu());
		((SNN *)args->snn_pointer) -> globalStateUpdate_CPU(args->netId);
#ifdef UNIX
		pthread_exit(0);
#else
		pthread_exit((void*) 0); // no dependends
		return NULL; // never reached
#endif
	}
#endif
*/

		ready = false;
		++count;

		pool->cv_.notify_all();
	}
} 


//ThreadPool::ThreadPool(int partitions, int cores, int offset)
//	: partitions_(partitions), cores_(cores), offset_(offset)
//{
//	for (int p = 0; p < partitions_; ++p) {
//		Worker* worker = new Worker;
//		worker->core_id = p % cores_ + offset_;
//		worker->pool = this;
//		worker->thread = new std::thread(&Worker::worker_thread, worker);
//#ifdef WIN32
//		SetThreadAffinityMask(worker->thread->native_handle(), 1ull << worker->core_id);
//#endif
//		workers_.push_back(worker);
//	}
//}
//

ThreadPool::ThreadPool(std::function<void(SNN*, int netId)> snn_method, 
	std::vector<ThreadStruct> partitions, int cores, int offset)
	: partitions_(partitions), cores_(cores), offset_(offset)
{
	for (int p = 0; p < partitions_.size(); ++p) {
		Worker* worker = new Worker;
		worker->snn_method = snn_method;
		worker->args = partitions_[p];
		//worker->core_id = p % cores_ + offset_;
		worker->core_id = p % cores_ * 2 + offset_;  // each physical partition has two logical
		worker->pool = this;
		worker->thread = new std::thread(&Worker::worker_thread, worker);
#ifdef WIN32
		SetThreadAffinityMask(worker->thread->native_handle(), 1ull << worker->core_id);
#endif
		workers_.push_back(worker);
	};

}

ThreadPool::~ThreadPool() {
	stop();
	for (auto w : workers_) {
		delete w->thread;
		delete w;
	}
}

void ThreadPool::next() {
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

void ThreadPool::stop() {
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


int ThreadPool::cores() const { 
	return cores_; 
}
