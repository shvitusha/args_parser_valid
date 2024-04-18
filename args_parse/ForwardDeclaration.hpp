#pragma once

#include <chrono>

//forward declarations
namespace args_parser {
	class SharedValidator;
	class ArgumentBase;
	template<typename T>
	class Validator;
	template<typename T>
	class Argument;
	class ArgsParser;
	enum class OperatorType;
	struct BaseParametrs;
}