#pragma once
#include "argument.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <tuple>

namespace args_parse {
	class ArgumentBase;

	/// @brief ѕеречисление типов оператора (длинный, короткий, неопределенный)
	enum class OperatorType {
		Long,
		Short,
		Nope
	};
	/// @brief —труктура дл€ определени€ параметров
	struct BaseParametrs {
		std::string_view argStr;
		std::string_view argName;
		std::string_view argValue;
	};

	class ArgsParser {
	public:
		/// @brief  онструктор класса.
		///  онструктор принимает количество аргументов и их значени€
		ArgsParser(int argc, const char** argv);

		/// @brief ƒобавление аргумента командной строки в вектор
		void Add(ArgumentBase* arg);

		/// @brief ѕарсинг аргументов командной строки.
		/// ѕроходит по каждому аргументу командной строки и провер€ет, был ли найден аргумент в векторе
		[[nodiscard]] bool Parse();

		/// @brief ¬ывод справки об использовании программы.
		/// ¬ыводит описание всех добавленных аргументов командной строки
		void ShowHelp() const;

		/// @brief ¬ывод дополнительной справки об использовании программы.s
		/// ¬ыводит описание всех добавленных аргументов, принимающих параметр, и как использовать.
		void ShowHelpVerbose() const;

		/// @brief ѕоиск аргумента.
		/// ¬ зависимости от оператора вызывает поиск короткого или длинного имени.
		[[nodiscard]] ArgumentBase* FindArgument(BaseParametrs param) const;

	private:
		/// @brief –азбор длинных аргументов командной строки.
		/// »звлекает им€ и значение аргумента дл€ дальнейшей обработки.
		[[nodiscard]] BaseParametrs ParseLongArgument(BaseParametrs p_param);

		/// @brief –азбор коротких аргументов командной строки.
		/// »звлекает им€ и значение аргумента дл€ дальнейшей обработки.
		[[nodiscard]] BaseParametrs ParseShortArgument(BaseParametrs p_param) const;

		/// @brief ќбработка одного аргумента командной строки.
		/// “акже провер€ет его наличие, наличие у него значени€, если да, то его проверку.
		void ProcessArgument(BaseParametrs p_param, int& i) const;

		/// @brief ¬алидаци€ значени€
		void ValidationValue(BaseParametrs p_param, ArgumentBase* arg, int& i) const;

		/// @brief ѕоиск длинного имени, если оно есть
		[[nodiscard]] ArgumentBase* FindLongNameArg(std::string_view item) const;

		/// @brief ѕоиск короткого имени, если оно есть
		[[nodiscard]] ArgumentBase* FindShortNameArg(std::string_view item) const;

		/// @brief ѕровер€ет €вл€етс€ ли строка оператором.
		/// ¬озвращает какой оператор был использован
		[[nodiscard]] OperatorType IsOperator(std::string_view operatString) const;

		/// —колько всего аргументов.
		int _argc;
		/// «начени€ аргументов. ƒолжно быть ровно @a argc.
		const char** _argv;
		/// ћассив аргументов командной строки
		std::vector<ArgumentBase*> _args;
	};
}