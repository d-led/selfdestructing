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

TEST_CASE("crashes after total instances") {
	struct TestAfterTotalNrCrash : public crashes::after_total<2,TestAfterTotalNrCrash>::instances {};
	{ TestAfterTotalNrCrash C; }
	CHECK_THROWS( TestAfterTotalNrCrash C );
}

TEST_CASE("crashes after total instances when copied") {
	struct TestAfterTotalNrCrash : public crashes::after_total<2,TestAfterTotalNrCrash>::instances {};
	TestAfterTotalNrCrash C;
	CHECK_THROWS( TestAfterTotalNrCrash C2=C );
}

TEST_CASE("crashes on copies by aggregation") {
	struct TestNumberCrashMember { crashes::on<2>::copies _; };
	TestNumberCrashMember C;
	TestNumberCrashMember C2=C;
	CHECK_THROWS( TestNumberCrashMember C3=C );
}

namespace {
	struct TestCopyNrCrashMember { crashes::after<2>::copies _; };
}

TEST_CASE("doesn't crash after copies by aggregation") {
	TestCopyNrCrashMember C;
	TestCopyNrCrashMember C2=C;
	CHECK_NOTHROW( TestCopyNrCrashMember C3=C );
}

TEST_CASE("crashes after copies by aggregation") {
	TestCopyNrCrashMember C;
	TestCopyNrCrashMember C2=C;
	CHECK_THROWS( TestCopyNrCrashMember C3=C2 );
}

namespace {
	struct TestTotalNrCrashMember { crashes::on_total<2,TestTotalNrCrashMember>::instances _; };
}

TEST_CASE("crashes on total by aggregation") {
	TestTotalNrCrashMember C;
	CHECK_THROWS( TestTotalNrCrashMember C2 );
}

TEST_CASE("doesn't crash on total by aggregation") {
	{ TestTotalNrCrashMember C; }
	TestTotalNrCrashMember C;
}

TEST_CASE("crashes on total by aggregation when copied") {
	TestTotalNrCrashMember C;
	CHECK_THROWS( TestTotalNrCrashMember C2=C );
}

TEST_CASE("crashes after total by aggregation") {
	struct TestAfterTotalNrCrashMember { crashes::after_total<2,TestAfterTotalNrCrashMember>::instances _; };
	{ TestAfterTotalNrCrashMember C; }
	CHECK_THROWS( TestAfterTotalNrCrashMember C );
}

TEST_CASE("crashes after total by aggregation when copied") {
	struct TestAfterTotalNrCrashMember { crashes::after_total<2,TestAfterTotalNrCrashMember>::instances _; };
	TestAfterTotalNrCrashMember C;
	CHECK_THROWS( TestAfterTotalNrCrashMember C2=C );
}
