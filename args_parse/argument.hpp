#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <optional>

namespace args_parse {
#pragma region Validation

	template<typename T>
	class Validator {
	public:
		Validator() = default;

		[[nodiscard]] std::tuple<bool, T> ValidValue(std::string_view value) const {
			std::string str = std::string(value);
			if (str.empty()) {
				return std::make_tuple(false, T{});
			}
			std::istringstream iss(str);
			T temp;
			if (iss >> temp) {
				return std::make_tuple(true, temp);
			}
			return std::make_tuple(false, temp);
		}
	};

	template<>
	class Validator<std::chrono::milliseconds> {
	public:
		Validator<std::chrono::milliseconds>() = default;

		[[nodiscard]] std::tuple<bool, std::chrono::milliseconds> ValidValue(std::string_view  value) const {
			long long l_value;
			std::string unit;
			std::string temp = std::string(value);
			std::istringstream ss{ temp };
			// Считываем значение и единицу измерения времени из потока
			ss >> l_value >> unit;

			std::chrono::milliseconds ms = std::chrono::milliseconds::zero();

			// Преобразуем значение и единицу измерения времени в микросекунды
			if (unit == "ms") {
				ms = std::chrono::milliseconds(l_value);
			}
			else if (unit == "s") {
				ms = std::chrono::seconds(l_value);
			}
			else {
				std::cerr << "Invalid time unit: " << unit << std::endl;
				return std::make_tuple(false, ms);
			}
			return std::make_tuple(true, ms);
		}
	};
#pragma endregion

	class ArgumentBase {
	public:
		/// @brief конструктор класса
		/// конструктор для случая, когда есть как короткое, так и длинное имя
		ArgumentBase(char shortName, const char* longName, bool isValue) :
			_shortName(shortName), _longName(longName), _isValue(isValue), _description(""), _isDefined(false) {}

		/// конструктор для случая, когда нет короткого имени
		ArgumentBase(const char* longName, bool isValue) : ArgumentBase('\0', longName, isValue) {}

		/// конструктор по умолчанию
		ArgumentBase() : ArgumentBase('\0', "", false) {}

		/// @brief проверка может ли быть у аргумента значение
		[[nodiscard]] bool HasValue() const { return _isValue; }

		/// @brief get() для получения значени¤ поля, соответствующего в классе
		[[nodiscard]] std::string GetLongName() const { return _longName; }

		/// @brief метод set() для присваивания значения полю, соответствующему в классе
		void SetLongName(const char* longName) { _longName = longName; }

		/// @brief get() для получения значения поля, соответствующего в классе
		[[nodiscard]] char GetShortName() const { return _shortName; }

		/// @brief метод set() для присваивания значения полю, соответствующему в классе
		void SetShortName(const char shortName) { _shortName = shortName; }

		/// @brief get() для получения значения поля, соответствующего в классе
		[[nodiscard]] std::string GetDescription() const { return _description; }

		/// @brief метод set() для присваивания значения полю, соответствующему в классе
		void SetDescription(const std::string& description) { _description = description; }

		/// @brief метод set() для присваивания значения полю, соответствующему в классе
		void SetIsDefined(const bool isDefined) { _isDefined = isDefined; }

		/// @brief get() для получения значения поля, соответствующего в классе
		[[nodiscard]] bool GetIsDefined() const { return _isDefined; }

		//[[nodiscard]] virtual const SharedValidator* GetValidator() const = 0;

		/// @brief метод для получения существует ли валидатор
		[[nodiscard]] virtual bool IsValidatorExist() const = 0;

		/// @brief метод для получения результата валидации и установки значения
		[[nodiscard]] virtual bool ValidationAndSetValue(std::string_view value) = 0;

		//virtual void SetValue(const std::string& value) = 0;

	private:
		///короткое описание аргумента
		char _shortName;
		///длинное описание аргумента
		std::string _longName;
		///дополнительное описание аргумента
		std::string _description;
		///флаг на наличие параметра
		bool _isValue;
		///флаг на определение аргумента
		bool _isDefined;
	};

	template<typename T>
	class Argument : public ArgumentBase {
	public:
		using ArgumentBase::ArgumentBase;

		/// @brief конструктор класса
		/// конструктор для случая, когда есть как короткое, так и длинное имя
		Argument(char shortName, const char* longName, bool isValue, Validator<T>* validator) :
			ArgumentBase(shortName, longName, isValue), _validator(validator) {}

		/// конструктор для случая, когда нет короткого имени
		Argument(const char* longName, bool isValue, Validator<T>* validator) :
			ArgumentBase('\0', longName, isValue), _validator(validator) {}

		/// @brief метод set() для присваивания значения полю, соответствующему в классе
		void SetValue(const std::optional<T>& value) {
			_value = value;
		}
		bool IsValidatorExist() const override
		{
			if (_validator == nullptr) return false;
			return true;
		}
		/// @brief get() для получения значения поля, соответстувющего в классе
		[[nodiscard]] std::optional<T> GetValue() const { return _value; }

		bool ValidationAndSetValue(std::string_view value)  override {
			const std::tuple<bool, T> valid_tuple = _validator->ValidValue(value);
			if (IsValidatorExist() && std::get<0>(valid_tuple)) {
				SetValue(std::get<1>(valid_tuple));
				return true;
			}
			return false;
		}

	protected:
		///значение аргумента
		std::optional<T> _value;
		/// валидатор
		Validator<T>* _validator;
	};

	template<>
	class Argument<std::chrono::milliseconds> : public ArgumentBase {
	private:
		///значение аргумента
		std::chrono::milliseconds _value;
		/// валидатор
		Validator<std::chrono::milliseconds>* _validator;

	public:
		/// @brief конструктор класса
		/// конструктор для случая, когда есть как короткое, так и длинное имя
		Argument<std::chrono::milliseconds>(char shortName, const char* longName, bool isValue, Validator<std::chrono::milliseconds>* validator = nullptr) :
			ArgumentBase(shortName, longName, isValue), _validator(validator), _value(std::chrono::milliseconds::zero()) {}

		/// конструктор для случая, когда нет короткого имени
		Argument<std::chrono::milliseconds>(const char* longName, bool isValue, Validator<std::chrono::milliseconds>* validator = nullptr) :
			ArgumentBase('\0', longName, isValue), _validator(validator), _value(std::chrono::milliseconds::zero()) {}

		using ArgumentBase::ArgumentBase;

		/// @brief метод set() для присваивания значения полю, соответствующему в классе
		void SetValue(const std::chrono::milliseconds& value) {
			_value = value;
		}

		bool IsValidatorExist() const override
		{
			if (_validator == nullptr) return false;
			return true;
		}

		bool ValidationAndSetValue(std::string_view value) override {
			if (!IsValidatorExist())
				return false;

			const std::tuple<bool, std::chrono::milliseconds> valid_tuple = _validator->ValidValue(value);
			if (std::get<0>(valid_tuple)) {
				SetValue(std::get<1>(valid_tuple));
				return true;
			}
			return false;
		}

		/// @brief get() для получения значения поля, соответствующего в классе
		[[nodiscard]] std::chrono::milliseconds GetValue() const { return _value; }
	};
}