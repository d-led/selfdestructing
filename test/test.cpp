#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <selfdestructing.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cassert>
#include <thread>
#include <future>
#include <chrono>

#define ASSERT( expression ) std::cout<<( (expression) ? "OK" : "NOT OK" )<<std::endl;

void test();

TEST_CASE("crashes on 2 copies, used via inheritance") {
	struct TestNumberCrash : public crashes::on<2>::copies {};
	TestNumberCrash C;
	CHECK_NOTHROW( [&]{ TestNumberCrash C2=C; }() );
	CHECK_THROWS( [&]{ TestNumberCrash C3=C; }() );
}

TEST_CASE("crashes after 2 copies") {
	struct TestCopyNrCrash : public crashes::after<2>::copies { TestCopyNrCrash() {} };
	TestCopyNrCrash C;
	TestCopyNrCrash C2=C;
	CHECK_NOTHROW( [&]{ TestCopyNrCrash C3=C; }() );

	////

	CHECK_THROWS( [&]{ TestCopyNrCrash C4=C2; }() );
}

namespace {
		struct TestTotalNrCrash : public crashes::on_total<2,TestTotalNrCrash>::instances {};
}

TEST_CASE("crashes on_total instances") {
	TestTotalNrCrash C;
	CHECK_THROWS( [&]{ TestTotalNrCrash C2; }() );
}

TEST_CASE("doesn't crashe on_total instances when destroyed") {
	{ TestTotalNrCrash C; }
	CHECK_NOTHROW( TestTotalNrCrash{} );
}

TEST_CASE("crashes on_total instances when copied") {
	TestTotalNrCrash C;
	CHECK_THROWS( TestTotalNrCrash(C) );
}

TEST_CASE("crashes after total instances") {
	struct TestAfterTotalNrCrash : public crashes::after_total<2,TestAfterTotalNrCrash>::instances {};
	{ TestAfterTotalNrCrash C; }
	CHECK_THROWS( TestAfterTotalNrCrash{} );
}

TEST_CASE("crashes after total instances when copied") {
	struct TestAfterTotalNrCrash : public crashes::after_total<2,TestAfterTotalNrCrash>::instances {};
	TestAfterTotalNrCrash C;
	CHECK_THROWS( TestAfterTotalNrCrash(C) );
}

TEST_CASE("crashes on copies by aggregation") {
	struct TestNumberCrashMember { crashes::on<2>::copies _; };
	TestNumberCrashMember C;
	TestNumberCrashMember C2=C;
	CHECK_THROWS( TestNumberCrashMember(C) );
}

namespace {
	struct TestCopyNrCrashMember { crashes::after<2>::copies _; };
}

TEST_CASE("doesn't crash after copies by aggregation") {
	TestCopyNrCrashMember C;
	TestCopyNrCrashMember C2=C;
	CHECK_NOTHROW( TestCopyNrCrashMember(C) );
}

TEST_CASE("crashes after copies by aggregation") {
	TestCopyNrCrashMember C;
	TestCopyNrCrashMember C2=C;
	CHECK_THROWS( TestCopyNrCrashMember(C2) );
}

namespace {
	struct TestTotalNrCrashMember { crashes::on_total<2,TestTotalNrCrashMember>::instances _; };
}

TEST_CASE("crashes on total by aggregation") {
	TestTotalNrCrashMember C;
	CHECK_THROWS( TestTotalNrCrashMember{} );
}

TEST_CASE("doesn't crash on total by aggregation") {
	{ TestTotalNrCrashMember C; }
	TestTotalNrCrashMember C;
}

TEST_CASE("crashes on total by aggregation when copied") {
	TestTotalNrCrashMember C;
	CHECK_THROWS( TestTotalNrCrashMember(C) );
}

TEST_CASE("crashes after total by aggregation") {
	struct TestAfterTotalNrCrashMember { crashes::after_total<2,TestAfterTotalNrCrashMember>::instances _; };
	{ TestAfterTotalNrCrashMember C; }
	CHECK_THROWS( TestAfterTotalNrCrashMember{} );
}

TEST_CASE("crashes after total by aggregation when copied") {
	struct TestAfterTotalNrCrashMember { crashes::after_total<2,TestAfterTotalNrCrashMember>::instances _; };
	TestAfterTotalNrCrashMember C;
	CHECK_THROWS( TestAfterTotalNrCrashMember(C) );
}

TEST_CASE("multithreaded crashes on copies") {
	struct TestMultiThreadCrash : public crashes::on<3>::copies {};
	
	TestMultiThreadCrash original;
	std::atomic<int> success_count{0};
	std::atomic<int> exception_count{0};
	
	auto copy_task = [&]() {
		try {
			TestMultiThreadCrash copy = original;
			success_count++;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		} catch (const std::exception&) {
			exception_count++;
		}
	};
	
	std::vector<std::thread> threads;
	for (int i = 0; i < 5; ++i) {
		threads.emplace_back(copy_task);
	}
	
	for (auto& t : threads) {
		t.join();
	}
	
	CHECK(success_count == 2);
	CHECK(exception_count == 3);
}

TEST_CASE("multithreaded crashes after copies") {
	struct TestMultiThreadAfterCrash : public crashes::after<2>::copies {};
	
	TestMultiThreadAfterCrash original;
	std::atomic<int> success_count{0};
	std::atomic<int> exception_count{0};
	
	auto copy_task = [&]() {
		try {
			TestMultiThreadAfterCrash copy = original;
			success_count++;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		} catch (const std::exception&) {
			exception_count++;
		}
	};
	
	std::vector<std::thread> threads;
	for (int i = 0; i < 4; ++i) {
		threads.emplace_back(copy_task);
	}
	
	for (auto& t : threads) {
		t.join();
	}
	
	CHECK(success_count == 4);
	CHECK(exception_count == 0);
}

TEST_CASE("multithreaded crashes on total instances") {
	struct TestMultiThreadTotalCrash : public crashes::on_total<3,TestMultiThreadTotalCrash>::instances {};
	
	std::atomic<int> success_count{0};
	std::atomic<int> exception_count{0};
	
	auto create_task = [&]() {
		try {
			TestMultiThreadTotalCrash instance;
			success_count++;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		} catch (const std::exception&) {
			exception_count++;
		}
	};
	
	std::vector<std::thread> threads;
	for (int i = 0; i < 6; ++i) {
		threads.emplace_back(create_task);
	}
	
	for (auto& t : threads) {
		t.join();
	}
	
	CHECK(success_count == 2);
	CHECK(exception_count == 4);
}

TEST_CASE("multithreaded crashes after total instances") {
    struct TestMultiThreadAfterTotalCrash : public crashes::after_total<3,TestMultiThreadAfterTotalCrash>::instances {};
    
    std::atomic<int> success_count{0};
    std::atomic<int> exception_count{0};
    
    auto create_task = [&]() {
        try {
            TestMultiThreadAfterTotalCrash instance;
            success_count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } catch (const std::exception&) {
            exception_count++;
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 6; ++i) {
        threads.emplace_back(create_task);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    CHECK(success_count == 2);
    CHECK(exception_count == 4);
    CHECK(success_count + exception_count == 6);
}

TEST_CASE("multithreaded aggregation crash test") {
	struct TestMultiThreadMemberCrash { 
		crashes::on<3>::copies _; 
	};
	
	TestMultiThreadMemberCrash original;
	std::atomic<int> success_count{0};
	std::atomic<int> exception_count{0};
	
	auto copy_task = [&]() {
		try {
			TestMultiThreadMemberCrash copy = original;
			success_count++;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		} catch (const std::exception&) {
			exception_count++;
		}
	};
	
	std::vector<std::thread> threads;
	for (int i = 0; i < 5; ++i) {
		threads.emplace_back(copy_task);
	}
	
	for (auto& t : threads) {
		t.join();
	}
	
	CHECK(success_count == 2);
	CHECK(exception_count == 3);
}

TEST_CASE("multithreaded race condition stress test") {
	struct StressTestCrash : public crashes::on<10>::copies {};
	
	StressTestCrash original;
	std::atomic<int> success_count{0};
	std::atomic<int> exception_count{0};
	
	auto copy_task = [&]() {
		for (int i = 0; i < 10; ++i) {
			try {
				StressTestCrash copy = original;
				success_count++;
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			} catch (const std::exception&) {
				exception_count++;
			}
		}
	};
	
	std::vector<std::thread> threads;
	for (int i = 0; i < 20; ++i) {
		threads.emplace_back(copy_task);
	}
	
	for (auto& t : threads) {
		t.join();
	}
	
	// Total attempts: 20 threads * 10 attempts = 200
	// Should have 9 successes and 191 exceptions (since crashes::on<10> allows 9 copies)
	CHECK(success_count == 9);
	CHECK(exception_count == 191);
}

TEST_CASE("multithreaded sequential vs parallel copy behavior") {
	// Test sequential copying (should be predictable)
	{
		struct SequentialCopyTest : public crashes::on<3>::copies {};
		SequentialCopyTest original;
		
		int sequential_successes = 0;
		int sequential_exceptions = 0;
		
		for (int i = 0; i < 5; ++i) {
			try {
				SequentialCopyTest copy = original;
				sequential_successes++;
			} catch (const std::exception&) {
				sequential_exceptions++;
			}
		}
		
		CHECK(sequential_successes == 2);
		CHECK(sequential_exceptions == 3);
	}
	
	// Probe parallel copying for breaking race conditions
	{
		struct ParallelCopyTest : public crashes::on<3>::copies {};
		ParallelCopyTest original;
		
		std::atomic<int> parallel_successes{0};
		std::atomic<int> parallel_exceptions{0};
		
		auto copy_task = [&]() {
			try {
				ParallelCopyTest copy = original;
				parallel_successes++;
			} catch (const std::exception&) {
				parallel_exceptions++;
			}
		};
		
		std::vector<std::thread> threads;
		for (int i = 0; i < 5; ++i) {
			threads.emplace_back(copy_task);
		}
		
		for (auto& t : threads) {
			t.join();
		}
		
		CHECK(parallel_successes + parallel_exceptions == 5);
		CHECK(parallel_successes == 2);
		CHECK(parallel_exceptions == 3);
	}
}

TEST_CASE("multithreaded feedback mechanism test") {
    struct FeedbackTest : public crashes::on<4>::copies {};
    
    std::vector<int> feedback_values;
    std::mutex feedback_mutex;
    std::atomic<int> success_count{0};
    std::atomic<int> exception_count{0};
    
    auto feedback_fn = [&](int count) {
        std::lock_guard<std::mutex> lock(feedback_mutex);
        feedback_values.push_back(count);
    };
    
    FeedbackTest original;
    original.set_feedback(feedback_fn);
    
    auto copy_task = [&]() {
        try {
            FeedbackTest copy = original;
            success_count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } catch (const std::exception&) {
            exception_count++;
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 6; ++i) {
        threads.emplace_back(copy_task);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Sort feedback values for deterministic checking
    std::sort(feedback_values.begin(), feedback_values.end());
    
    CHECK(success_count == 3);
    CHECK(exception_count == 3);
    CHECK(success_count + exception_count == 6);
    
    CHECK(feedback_values.size() == 6);
    
    std::vector<int> expected_feedback = {1, 2, 3, 4, 5, 6};
    CHECK(feedback_values == expected_feedback);
}
