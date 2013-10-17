// selfdestructing.cpp : Defines the entry point for the console application.
//
#include <selfdestructing.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cassert>

#define ASSERT( expression ) std::cout<<( (expression) ? "OK" : "NOT OK" )<<std::endl;

int main(int argc, char* argv[])
{
	std::vector<std::pair<std::function<void()>,bool>> tests;

	// testing use by inheritance
	struct TestNumberCrash : public crashes::on<2>::copies {};
	tests.push_back(std::make_pair([]{
		TestNumberCrash C;
		C.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestNumberCrash C2=C;
		TestNumberCrash C3=C;
	},true));


	struct TestCopyNrCrash : public crashes::after<2>::copies {};
	tests.push_back(std::make_pair([]{
		TestCopyNrCrash C;
		C.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestCopyNrCrash C2=C;
		TestCopyNrCrash C3=C;
	},false));

	tests.push_back(std::make_pair([]{
		TestCopyNrCrash C;
		C.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestCopyNrCrash C2=C;
		TestCopyNrCrash C3=C2;
	},true));


	struct TestTotalNrCrash : public crashes::on_total<2,TestTotalNrCrash>::instances {};
	tests.push_back(std::make_pair([]{
		TestTotalNrCrash C;
		TestTotalNrCrash C2;
	},true));

	tests.push_back(std::make_pair([]{
		{ TestTotalNrCrash C; }
		TestTotalNrCrash C;
	},false));

	tests.push_back(std::make_pair([]{
		TestTotalNrCrash C;
		TestTotalNrCrash C2=C;
	},true));


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

	return 0;
}

