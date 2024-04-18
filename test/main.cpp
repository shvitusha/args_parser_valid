//#include <catch2/catch_all.hpp>
//
//#include <args_parse/argument.hpp>
//#include <args_parse/ArgsParser.hpp>
//
//#include <iostream>
//#include <memory>
//
//TEST_CASE("Parse value", "[ArgsParser]") {
//	args_parse::Argument<bool> help('h', "help", false);
//	args_parse::Argument<bool> verbose('v', "verbose", false);
//	args_parse::Argument<std::string> output('o', "output", true);
//	args_parse::Argument<int> number('n', "number", true);
//
//	const char* argv[] = { "program", "-h", "--output=file.txt", "-n25", "--number = 2", "--string", "--output - path = path" };
//	int argc = 7;
//	args_parse::ArgsParser parser(argc, argv);
//	parser.Add(&help);
//	parser.Add(&verbose);
//	parser.Add(&output);
//	parser.Add(&number);
//
//	SECTION("Parsing arguments") {
//		//REQUIRE(parser.Parse() == true);
//
//		REQUIRE_THROWS_AS(parser.Parse(), std::invalid_argument);
//	}
//
//	args_parse::Argument<bool> newArg('t', "test", false);
//	parser.Add(&newArg);
//
//	SECTION("Find new added argument") {
//		//REQUIRE(parser.FindArgument("t") != nullptr);
//		REQUIRE(parser.FindArgument("te") != nullptr);
//		REQUIRE(parser.FindArgument("test") != nullptr);
//	}
//
//	args_parse::Validator<int>* IntValidator = new args_parse::Validator<int>();
//
//	SECTION("Valid integer values") {
//		REQUIRE(IntValidator->ValidValue("10") == true);
//		REQUIRE(IntValidator->ValidValue("-5") == true);
//	}
//
//	SECTION("Invalid integer values") {
//		REQUIRE(IntValidator->ValidValue("") == false);
//		//REQUIRE(IntValidator->ValidValue("-8.92") == false);
//		REQUIRE(IntValidator->ValidValue("abc") == false);
//	}
//
//	args_parse::Validator<unsigned int>* unsignedIntValidator = new args_parse::Validator<unsigned int>();
//
//	SECTION("Invalid unsigned integer values") {
//		REQUIRE(unsignedIntValidator->ValidValue("") == false);
//		REQUIRE(unsignedIntValidator->ValidValue("abc") == false);
//		//REQUIRE(unsignedIntValidator->ValidValue("-5") == false);
//	}
//
//	args_parse::Validator<float>* floatValidator = new args_parse::Validator<float>();
//
//	SECTION("Valid float values") {
//		REQUIRE(floatValidator->ValidValue("10.56") == true);
//		REQUIRE(floatValidator->ValidValue("10,56") == true);
//		REQUIRE(floatValidator->ValidValue("-5") == true);
//	}
//
//	SECTION("Invalid float values") {
//		REQUIRE(floatValidator->ValidValue("") == false);
//		REQUIRE(floatValidator->ValidValue("abc") == false);
//	}
//
//	args_parse::Validator<std::string>* StringValidator = new args_parse::Validator<std::string>();
//
//	SECTION("Valid string values") {
//		REQUIRE(StringValidator->ValidValue("hello") == true);
//		REQUIRE(StringValidator->ValidValue("world") == true);
//	}
//
//	SECTION("Invalid string values") {
//		REQUIRE(StringValidator->ValidValue("") == false);
//	}
//
//	SECTION("Finding existing arguments") {
//		//REQUIRE(parser.FindArgument("h") != nullptr);
//		REQUIRE(parser.FindArgument("output") != nullptr);
//	}
//
//	args_parse::Argument<std::string> longArg("", "string");
//	parser.Add(&longArg);
//	args_parse::Argument<std::string> nonUnique("", "output-path");
//
//	SECTION("Non-unique argument addition") {
//		REQUIRE_THROWS_AS(parser.Add(&nonUnique), std::invalid_argument);
//	}
//
//	SECTION("Finding non-existing arguments") {
//		//REQUIRE_THROWS_AS(parser.FindArgument("x"), std::invalid_argument);
//		//REQUIRE_THROWS_AS(parser.FindArgument("invalid"), std::invalid_argument);
//		//REQUIRE_THROWS_AS(parser.FindArgument("s"), std::invalid_argument);
//	}
//
//
//}