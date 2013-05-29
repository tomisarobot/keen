
#include <UnitTest++.h>

#include <keen/args.h>

namespace keen {
namespace {

	struct builder {
		builder& add(const std::string& arg) { args.push_back(arg); }
		operator std::vector<std::string>() const { return args; }
		std::vector<std::string> args;
	};

	TEST(check_flag) {
		std::vector<std::string> args = builder()
			.add("--a=1")
			.add("--b=2")
			.add("--c")
			;
		CHECK( !check_flag(args, "a") );
		CHECK( !check_flag(args, "b") );
		CHECK( check_flag(args, "c") );
		CHECK( !check_flag(args, "d") );
	}

	TEST(check_flag_value) {
		std::vector<std::string> args = builder()
			.add("--a=1")
			.add("--b=2")
			.add("--c")
			;
		std::string value;
		CHECK( check_flag_value(args, "a", value) );
		CHECK( check_flag_value(args, "b", value) );
		CHECK( !check_flag_value(args, "c", value) );
		CHECK( !check_flag_value(args, "d", value) );
	}

	TEST(get_flag_value) {
		std::vector<std::string> args = builder()
			.add("--a=1")
			.add("--b=2")
			.add("--c")
			;
		CHECK_EQUAL( "1", get_flag_value(args, "a") );
		CHECK_EQUAL( "2", get_flag_value(args, "b") );
		CHECK_THROW( get_flag_value(args, "c"), std::runtime_error );
		CHECK_THROW( get_flag_value(args, "d"), std::runtime_error );
	}

	TEST(get_flag_value_or_default) {
		std::vector<std::string> args = builder()
			.add("--a=1")
			.add("--b=2")
			.add("--c")
			;
		CHECK_EQUAL( "1", get_flag_value(args, "a", "?") );
		CHECK_EQUAL( "2", get_flag_value(args, "b", "?") );
		CHECK_EQUAL( "?", get_flag_value(args, "c", "?") );
		CHECK_EQUAL( "?", get_flag_value(args, "d", "?") );
	}

}} // keen anon

