#include "gtest/gtest.h"
#include "pf/Promise.h"
#include <thread>
#include <iostream>

TEST(pf, simple)
{
	std::cout << "Zero\n";

	auto p1 = pf::Promise<int>([](auto resolve, auto reject) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::cout << "1\n";
		resolve(1);
	});

	std::cout << "One\n";

	auto p2 = p1.then<int>([](auto val) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::cout << "2\n";
		return val + 1;
	});

	std::cout << "Two\n";

	p2->finally([](auto val) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::cout << std::to_string(val + 1) << "\n";
	});

	std::cout << "Three\n";
}

TEST(pf, allTuple)
{
	auto p1 = pf::promise<int>([](auto resolve, auto reject) {
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		std::cout << "1\n";
		resolve(1);
	});

	auto p2 = pf::promise<double>([](auto resolve, auto reject) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "2.0\n";
		resolve(2.0);
	});

	auto p3 = pf::promise<std::string>([](auto resolve, auto reject) {
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		std::cout << "three\n";
		resolve("three");
	});

	auto all = pf::all(p1, p2, p3);
	all->finally([](const auto& values) {
		const auto& [val1, val2, val3] = values;
		std::cout << val1 << " "
			<< val2 << " "
			<< val3 << "\n";
	});
}

TEST(pf, allVector)
{
	std::vector<pf::SharedPromise<int>> container;
	std::cout << "Zero\n";

	container.push_back(pf::promise<int>([](auto resolve, auto reject) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::cout << "1\n";
		resolve(1);
	}));

	std::cout << "One\n";

	container.push_back(pf::promise<int>([](auto resolve, auto reject) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		std::cout << "2\n";
		resolve(2);
	}));

	std::cout << "Two\n";

	container.push_back(pf::promise<int>([](auto resolve, auto reject) {
		std::this_thread::sleep_for(std::chrono::milliseconds(400));
		std::cout << "3\n";
		resolve(3);
	}));

	std::cout << "Three\n";
	
	auto res = pf::all(container);
	res->finally([](const auto& values) {
		for (const auto& val : values)
			std::cout << val << " ";
		std::cout << "\n";
	});
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
