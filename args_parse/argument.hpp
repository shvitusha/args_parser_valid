#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <optional>
#include <filesystem>

namespace args_parse {
#pragma region Validation

	template<typename T>
	class Validator {
	public:
		Validator() = default;

		[[nodiscard]] std::tuple<bool, T> ValidValue(std::string_view value) const {
			std::string str = std::string(value);
			//строка может быть пустой
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
			//единица измерения
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

	template<>
	class Validator<std::string> {
	public:
		Validator<std::string>() = default;

		[[nodiscard]] std::tuple<bool, std::string> ValidValue(std::string_view value) const {
			std::filesystem::path dirPath = std::string(value);

			// Проверяем, существует ли каталог
			if (!std::filesystem::exists(dirPath))
				return std::make_tuple(false, std::string{});

			// Проверяем, является ли путь директорией
			if (!std::filesystem::is_directory(dirPath))
				return std::make_tuple(false, std::string{});

			// Проверяем, доступен ли каталог для чтения
			if (std::filesystem::directory_iterator(dirPath) == std::filesystem::directory_iterator())
				return std::make_tuple(false, std::string{});

			return std::make_tuple(true, dirPath.string());
		}
	};

#pragma endregion

	class ArgumentBase {
	public:
		/// @brief Конструктор класса
		/// Конструктор для случая, когда есть как короткое, так и длинное имя
		ArgumentBase(char shortName, const char* longName, bool isValue) :
			_shortName(shortName), _longName(longName), _isValue(isValue), _description(""), _isDefined(false) {}

		/// Конструктор для случая, когда нет короткого имени
		ArgumentBase(const char* longName, bool isValue) : ArgumentBase('\0', longName, isValue) {}

		/// Конструктор по умолчанию
		ArgumentBase() : ArgumentBase('\0', "", false) {}

		/// @brief Проверка может ли быть у аргумента значение
		[[nodiscard]] bool HasValue() const { return _isValue; }

		/// @brief Получение длинного имени
		[[nodiscard]] std::string GetLongName() const { return _longName; }

		/// @brief Установка длинного имени
		void SetLongName(const char* longName) { _longName = longName; }

		/// @brief Получение короткого имени
		[[nodiscard]] char GetShortName() const { return _shortName; }

		/// @brief Установка короткого имени
		void SetShortName(const char shortName) { _shortName = shortName; }

		/// @brief Получение описания аргумента
		[[nodiscard]] std::string GetDescription() const { return _description; }

		/// @brief Установка описания аргумента
		void SetDescription(const std::string& description) { _description = description; }

		/// @brief Установка определения аргумента
		void SetIsDefined(const bool isDefined) { _isDefined = isDefined; }

		/// @brief Проверка определения аргумента
		[[nodiscard]] bool GetIsDefined() const { return _isDefined; }

		/// @brief Проверка существования валидатора
		[[nodiscard]] virtual bool IsValidatorExist() const = 0;

		/// @brief Получение результата валидации и установка значения
		[[nodiscard]] virtual bool ValidationAndSetValue(std::string_view value) = 0;

	private:
		///Короткое описание аргумента
		char _shortName;
		///Длинное описание аргумента
		std::string _longName;
		///Дополнительное описание аргумента
		std::string _description;
		///Флаг на наличие параметра
		bool _isValue;
		///Флаг на определение аргумента
		bool _isDefined;
	};

	template<typename T>
	class Argument : public ArgumentBase {
	public:
		using ArgumentBase::ArgumentBase;

		/// @brief Конструктор класса
		/// Конструктор для случая, когда есть как короткое, так и длинное имя
		Argument(char shortName, const char* longName, bool isValue, Validator<T>* validator) :
			ArgumentBase(shortName, longName, isValue), _validator(validator) {}

		/// Конструктор для случая, когда нет короткого имени
		Argument(const char* longName, bool isValue, Validator<T>* validator) :
			ArgumentBase('\0', longName, isValue), _validator(validator) {}

		/// @brief Установка значения аргументу
		void SetValue(const std::optional<T>& value) {
			_value = value;
		}
		
		/// @brief Проверяет определен ли валидатор
		bool IsValidatorExist() const override
		{
			if (_validator == nullptr) return false;
			return true;
		}

		/// @brief Получение значения аргумента
		[[nodiscard]] std::optional<T> GetValue() const { return _value; }

		bool ValidationAndSetValue(std::string_view value)  override {
			const std::tuple<bool, T> valid_tuple = _validator->ValidValue(value);
			if (std::get<0>(valid_tuple)) {
				SetValue(std::get<1>(valid_tuple));
				return true;
			}
			return false;
		}

	protected:
		///Значение аргумента
		std::optional<T> _value;
		///Валидатор
		Validator<T>* _validator;
	};

	template<>
	class Argument<std::chrono::milliseconds> : public ArgumentBase {
	private:
		///Значение аргумента
		std::chrono::milliseconds _value;
		///Валидатор
		Validator<std::chrono::milliseconds>* _validator;

	public:
		/// @brief Конструктор класса
		/// Конструктор для случая, когда есть как короткое, так и длинное имя
		Argument<std::chrono::milliseconds>(char shortName, const char* longName, bool isValue, Validator<std::chrono::milliseconds>* validator = nullptr) :
			ArgumentBase(shortName, longName, isValue), _validator(validator), _value(std::chrono::milliseconds::zero()) {}

		/// Конструктор для случая, когда нет короткого имени
		Argument<std::chrono::milliseconds>(const char* longName, bool isValue, Validator<std::chrono::milliseconds>* validator = nullptr) :
			ArgumentBase('\0', longName, isValue), _validator(validator), _value(std::chrono::milliseconds::zero()) {}

		using ArgumentBase::ArgumentBase;

		/// @brief Установка значения аргументу
		void SetValue(const std::chrono::milliseconds& value) {
			_value = value;
		}

		/// @brief Проверяет определен ли валидатор
		bool IsValidatorExist() const override
		{
			if (_validator == nullptr) return false;
			return true;
		}

		bool ValidationAndSetValue(std::string_view value) override {
			const std::tuple<bool, std::chrono::milliseconds> valid_tuple = _validator->ValidValue(value);
			if (std::get<0>(valid_tuple)) {
				SetValue(std::get<1>(valid_tuple));
				return true;
			}
			return false;
		}

		/// @brief Получение значения аргумента
		[[nodiscard]] std::chrono::milliseconds GetValue() const { return _value; }
	};
}