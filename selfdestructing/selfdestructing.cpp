// selfdestructing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "crash_on_copy.hpp"
#include <iostream>

struct TestNumberCrash : public crashes::on<2>::copies
{
};

int _tmain(int argc, _TCHAR* argv[])
{
	try {
		TestNumberCrash C;
		C.set_feedback([](int num) { std::cout<<num<<std::endl; });
		TestNumberCrash C2=C;
		TestNumberCrash C3=C;
	} catch (std::exception& e) {
		std::cout<<e.what()<<std::endl;
	}

	return 0;
}

