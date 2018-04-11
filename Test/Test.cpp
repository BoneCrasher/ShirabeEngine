// Example program
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <future>

#include "Tests/Test_FrameGraph.h"

class Task {
public:

	std::future<int> bind(std::function<int()> fn) {
		m_task = std::packaged_task<int()>(fn);
		return m_task.get_future();
	}

	bool run() {
		m_task();
		return true;
	}

private:
	std::packaged_task<int()> m_task;
};


int main()
{
	/*uint32_t sleep = 6000;

	std::vector<std::future<int>> futures;
	std::vector<Task> tasks;

	uint32_t i = 5;
	while( i-- > 0 ) {
		Task t;
		uint32_t k = i;
		futures.push_back(std::move(t.bind([=] () -> int { return  k * 11; })));
		tasks.push_back(std::move(t));
	}

	std::thread thread = std::thread([&] () -> void {
		for( Task& task : tasks ) {
			std::cout << "Sleeping for " << sleep << " milliseconds" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
			std::cout << "Calculating..." << std::endl;
			task.run();
		}
	});

	for(std::future<int>& future : futures)
		std::cout << "Result: " << future.get() << std::endl;

	thread.join();*/

  // Test::FrameGraph::Test__FrameGraph test_framegraph{};
  // test_framegraph.testAll();

	system("Pause");
}