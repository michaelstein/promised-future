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

TEST(pf, multiple)
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
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
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

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
