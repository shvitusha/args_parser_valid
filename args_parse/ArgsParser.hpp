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

	/// @brief Перечисление типов оператора (длинный, короткий, неопределенный)
	enum class OperatorType {
		Long,
		Short,
		Nope
	};
	/// @brief Структура для определения параметров
	struct BaseParametrs {
		std::string_view argStr;
		std::string_view argName;
		std::string_view argValue;
	};

	class ArgsParser {
	public:
		/// @brief Конструктор класса.
		/// Конструктор принимает количество аргументов и их значения
		ArgsParser(int argc, const char** argv);

		/// @brief Добавление аргумента командной строки в вектор
		void Add(ArgumentBase* arg);

		/// @brief Парсинг аргументов командной строки.
		/// Проходит по каждому аргументу командной строки и проверяет, был ли найден аргумент в векторе
		[[nodiscard]] bool Parse();

		/// @brief Вывод справки об использовании программы.
		/// Выводит описание всех добавленных аргументов командной строки
		void ShowHelp() const;

		/// @brief Вывод дополнительной справки об использовании программы.
		/// Выводит описание всех добавленных аргументов, принимающих параметр, и как использовать.
		void ShowHelpVerbose() const;

		/// @brief Поиск аргумента.
		/// В зависимости от оператора вызывает поиск короткого или длинного имени.
		[[nodiscard]] ArgumentBase* FindArgument(BaseParametrs param) const;

	private:
		/// @brief Разбор длинных аргументов командной строки.
		/// Извлекает имя и значение аргумента для дальнейшей обработки.
		[[nodiscard]] BaseParametrs ParseLongArgument(BaseParametrs p_param);

		/// @brief Разбор коротких аргументов командной строки.
		/// Извлекает имя и значение аргумента для дальнейшей обработки.
		[[nodiscard]] BaseParametrs ParseShortArgument(BaseParametrs p_param) const;

		/// @brief Обработка одного аргумента командной строки.
		/// Также проверяет его наличие, наличие у него значени¤, если да, то его проверку.
		void ProcessArgument(BaseParametrs p_param, int& i) const;

		/// @brief Валидация значения
		void ValidationValue(BaseParametrs p_param, ArgumentBase* arg, int& i) const;

		/// @brief Поиск длинного имени, если оно есть
		[[nodiscard]] ArgumentBase* FindLongNameArg(std::string_view item) const;

		/// @brief Поиск короткого имени, если оно есть
		[[nodiscard]] ArgumentBase* FindShortNameArg(std::string_view item) const;

		/// @brief Проверяет является ли строка оператором.
		/// Возвращает какой оператор был использован
		[[nodiscard]] OperatorType IsOperator(std::string_view operatString) const;

		/// Сколько всего аргументов.
		int _argc;
		/// Значения аргументов. Должно быть ровно @a argc.
		const char** _argv;
		/// Вектор аргументов командной строки
		std::vector<ArgumentBase*> _args;
	};
}