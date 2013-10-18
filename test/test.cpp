#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <selfdestructing.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cassert>

#define ASSERT( expression ) std::cout<<( (expression) ? "OK" : "NOT OK" )<<std::endl;

void test();

TEST_CASE("crashes on 2 copies, used via inheritance") {
	struct TestNumberCrash : public crashes::on<2>::copies {};
	TestNumberCrash C;
	CHECK_NOTHROW( TestNumberCrash C2=C );
	CHECK_THROWS( TestNumberCrash C3=C );
}

TEST_CASE("crashes after 2 copies") {
	struct TestCopyNrCrash : public crashes::after<2>::copies { TestCopyNrCrash() {} };
	TestCopyNrCrash C;
	TestCopyNrCrash C2=C;
	CHECK_NOTHROW( TestCopyNrCrash C3=C );

	////

	CHECK_THROWS( TestCopyNrCrash C4=C2 );
}

namespace {
		struct TestTotalNrCrash : public crashes::on_total<2,TestTotalNrCrash>::instances {};
}

TEST_CASE("crashes on_total instances") {
	TestTotalNrCrash C;
	CHECK_THROWS( TestTotalNrCrash C2 );
}

TEST_CASE("doesn't crashe on_total instances when destroyed") {
	{ TestTotalNrCrash C; }
	CHECK_NOTHROW( TestTotalNrCrash C );
}

TEST_CASE("crashes on_total instances when copied") {
	TestTotalNrCrash C;
	CHECK_THROWS( TestTotalNrCrash C2=C );
}

void test()
{
	std::vector<std::pair<std::function<void()>,bool>> tests;

	struct TestAfterTotalNrCrash : public crashes::after_total<2,TestAfterTotalNrCrash>::instances {};
	tests.push_back(std::make_pair([]{
		{ TestAfterTotalNrCrash C; }
		TestAfterTotalNrCrash C;
	},true));

	tests.push_back(std::make_pair([]{
		TestAfterTotalNrCrash C;
		TestAfterTotalNrCrash C2=C;
	},true));


	// testing use by aggregation
	struct TestNumberCrashMember { crashes::on<2>::copies _; };
	tests.push_back(std::make_pair([]{
		TestNumberCrashMember C;
		C._.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestNumberCrashMember C2=C;
		TestNumberCrashMember C3=C;
	},true));


	struct TestCopyNrCrashMember { crashes::after<2>::copies _; };
	tests.push_back(std::make_pair([]{
		TestCopyNrCrashMember C;
		C._.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestCopyNrCrashMember C2=C;
		TestCopyNrCrashMember C3=C;
	},false));

	tests.push_back(std::make_pair([]{
		TestCopyNrCrashMember C;
		C._.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestCopyNrCrashMember C2=C;
		TestCopyNrCrashMember C3=C2;
	},true));


	struct TestTotalNrCrashMember { crashes::on_total<2,TestTotalNrCrashMember>::instances _; };
	tests.push_back(std::make_pair([]{
		TestTotalNrCrashMember C;
		TestTotalNrCrashMember C2;
	},true));

	tests.push_back(std::make_pair([]{
		{ TestTotalNrCrashMember C; }
		TestTotalNrCrashMember C;
	},false));

	tests.push_back(std::make_pair([]{
		TestTotalNrCrashMember C;
		TestTotalNrCrashMember C2=C;
	},true));


	struct TestAfterTotalNrCrashMember { crashes::after_total<2,TestAfterTotalNrCrashMember>::instances _; };
	tests.push_back(std::make_pair([]{
		{ TestAfterTotalNrCrashMember C; }
		TestAfterTotalNrCrashMember C;
	},true));

	tests.push_back(std::make_pair([]{
		TestAfterTotalNrCrashMember C;
		TestAfterTotalNrCrashMember C2=C;
	},true));

	for (auto test : tests) {
		bool crashed=false;
		try {
			test.first();
		} catch (std::exception& e) {
			crashed=true;
			std::cout<<e.what()<<std::endl;
		}
		ASSERT(crashed==test.second);
	}
}

