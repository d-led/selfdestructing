// selfdestructing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "crash_on_copy.hpp"
#include <iostream>
#include <vector>
#include <functional>
#include <cassert>

struct TestNumberCrash : public crashes::on<2>::copies {};
struct TestCopyNrCrash : public crashes::after<2>::copies {};
struct TestTotalNrCrash : public crashes::on_total<2,TestTotalNrCrash>::instances {};

int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<std::pair<std::function<void()>,bool>> tests;
	tests.push_back(std::make_pair([]{
		TestNumberCrash C;
		C.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestNumberCrash C2=C;
		TestNumberCrash C3=C;
	},true));

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

	for (auto test : tests) {
		bool crashed=false;
		try {
			test.first();
		} catch (std::exception& e) {
			crashed=true;
			std::cout<<e.what()<<std::endl;
		}
		assert(crashed==test.second);
	}

	return 0;
}

