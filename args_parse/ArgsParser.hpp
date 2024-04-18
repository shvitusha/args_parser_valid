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

	/// @brief ������������ ����� ��������� (�������, ��������, ��������������)
	enum class OperatorType {
		Long,
		Short,
		Nope
	};
	/// @brief ��������� ��� ����������� ����������
	struct BaseParametrs {
		std::string_view argStr;
		std::string_view argName;
		std::string_view argValue;
	};

	class ArgsParser {
	public:
		/// @brief ����������� ������.
		/// ����������� ��������� ���������� ���������� � �� ��������
		ArgsParser(int argc, const char** argv);

		/// @brief ���������� ��������� ��������� ������ � ������
		void Add(ArgumentBase* arg);

		/// @brief ������� ���������� ��������� ������.
		/// �������� �� ������� ��������� ��������� ������ � ���������, ��� �� ������ �������� � �������
		[[nodiscard]] bool Parse();

		/// @brief ����� ������� �� ������������� ���������.
		/// ������� �������� ���� ����������� ���������� ��������� ������
		void ShowHelp() const;

		/// @brief ����� �������������� ������� �� ������������� ���������.s
		/// ������� �������� ���� ����������� ����������, ����������� ��������, � ��� ������������.
		void ShowHelpVerbose() const;

		/// @brief ����� ���������.
		/// � ����������� �� ��������� �������� ����� ��������� ��� �������� �����.
		[[nodiscard]] ArgumentBase* FindArgument(BaseParametrs param) const;

	private:
		/// @brief ������ ������� ���������� ��������� ������.
		/// ��������� ��� � �������� ��������� ��� ���������� ���������.
		[[nodiscard]] BaseParametrs ParseLongArgument(BaseParametrs p_param);

		/// @brief ������ �������� ���������� ��������� ������.
		/// ��������� ��� � �������� ��������� ��� ���������� ���������.
		[[nodiscard]] BaseParametrs ParseShortArgument(BaseParametrs p_param) const;

		/// @brief ��������� ������ ��������� ��������� ������.
		/// ����� ��������� ��� �������, ������� � ���� ��������, ���� ��, �� ��� ��������.
		void ProcessArgument(BaseParametrs p_param, int& i) const;

		/// @brief ��������� ��������
		void ValidationValue(BaseParametrs p_param, ArgumentBase* arg, int& i) const;

		/// @brief ����� �������� �����, ���� ��� ����
		[[nodiscard]] ArgumentBase* FindLongNameArg(std::string_view item) const;

		/// @brief ����� ��������� �����, ���� ��� ����
		[[nodiscard]] ArgumentBase* FindShortNameArg(std::string_view item) const;

		/// @brief ��������� �������� �� ������ ����������.
		/// ���������� ����� �������� ��� �����������
		[[nodiscard]] OperatorType IsOperator(std::string_view operatString) const;

		/// ������� ����� ����������.
		int _argc;
		/// �������� ����������. ������ ���� ����� @a argc.
		const char** _argv;
		/// ������ ���������� ��������� ������
		std::vector<ArgumentBase*> _args;
	};
}