// selfdestructing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "selfdestructs_itself.hpp"
#include <iostream>

struct TestSelfDestructs : public selfdestructs::on<1>::copy
{
};

int _tmain(int argc, _TCHAR* argv[])
{
	TestSelfDestructs C;
	TestSelfDestructs C2=C;

	return 0;
}

