#include <iostream>
#include <thread>
#include <brpc/server.h>
#include <chrono>
#include <mutex>
#include <atomic>
using namespace std;

std::mutex g_mt;
const int nNum = 10;
/*
//std::atomic<long long> ticket(900000l);
long long ticket = 900000;
void fun(){
	while(1){
		{
			std::lock_guard<std::mutex> lock(g_mt);
			if(ticket < 0) break;
			std::cout << std::this_thread::get_id() <<" : "<< ticket-- << endl;
		}
//		if(ticket.load(std::memory_order_relaxed) < 0) break;
//		ticket.fetch_sub(1, std::memory_order_relaxed);
//		std::cout << std::this_thread::get_id() <<" : "<< ticket.load(std::memory_order_relaxed) << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	};
}
*/

int a = 1;
class slock{
public:
	slock(){
		while(!__sync_bool_compare_and_swap(&a, 1, 0)){
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	~slock(){
		__sync_bool_compare_and_swap(&a, 0, 1);
	}
};

long long ticket = 900000;

void fun2(){
		while(1){
			{
				slock s;
				if(ticket < 0) break;
				std::cout << std::this_thread::get_id() <<" : "<< ticket-- << std::endl;	
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
}

int main(){
	brpc::StartDummyServerAt(8888/*port*/);
	thread td[nNum];
	time_t t1 = time(nullptr);
	for(auto i = 0; i < nNum; i++){
		td[i] = thread(fun2);
	}
	for(auto i = 0; i < nNum; i++){
		td[i].join();
	}
	time_t t2 = time(nullptr);
	std::cout << " time = "<<t2 - t1 << std::endl;
	return 0;
}
