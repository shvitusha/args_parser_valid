#include "args_parse/argument.hpp"
#include "args_parse/ArgsParser.hpp"
#include <thread>
#include <vector>
#include <filesystem>
#include <chrono>
#include <queue>
#include <mutex>
#include <functional>

struct Directory {
	///id потока, обрабатывающего текущую директорию
	std::thread::id _threadId;
	
	///Путь директории
	std::filesystem::path _path;
	
	///Вектор всех файлов текущей директории
	std::vector<std::filesystem::path> _filenames;
	
	///Вектор всех поддиректорий в директории
	std::vector<Directory> _directories;

	Directory(const std::filesystem::path path) : _path{ path } {}

	///@brief Получение пути
	[[nodiscard]] const std::filesystem::path& GetPath() const { return _path; }

	///@brief Установка пути
	void SetPath(const std::filesystem::path path) { _path = path; }

	///@brief Добавление файла в вектор всех файлов
	void AddFile(const std::filesystem::path& file)
	{
		_filenames.push_back(file);
	}

	///@brief Добавление файла в вектор всех директорий
	void AddDirectory(const std::filesystem::path& file) {
		_directories.push_back(Directory(file));
	}
};

struct ThreadPool {
	/// Количество потоков
	unsigned int _threadPool;

	/// Количество заморозки в миллисекундах
	std::chrono::milliseconds _debugSleep;
	
	///Очередь
	std::queue<std::function<void()>> _tasks;
	
	/// Мьютекст для очереди
	std::mutex _taskMutex;
	
	///Условная переменная для очереди
	std::condition_variable _taskCV;
	
	/// Мьютекст для пула
	std::mutex _poolMutex;
	
	///Вектор потоков
	std::vector<std::thread> _threads;
	bool _stop;
	
	///@brief Конструктор класса ThreadPool
	ThreadPool(unsigned int threadPool, std::chrono::milliseconds debugSleep) :
		_threadPool(threadPool), _debugSleep(std::move(debugSleep)), _stop(false) {
		// Создание потоков в пуле
		for (unsigned int i = 0; i < _threadPool; ++i) {
			_threads.emplace_back(std::bind(&ThreadPool::WorkerThread, this));
		}
	}

	///@brief Деструктор класса ThreadPool
	~ThreadPool() {
		{
			// Захватываем мьютекс для безопасного изменения переменной _stop
			std::lock_guard<std::mutex> lock(_taskMutex);
			_stop = true; // Устанавливаем флаг остановки
		}
		_taskCV.notify_all(); // Уведомляем все потоки о завершении
		// Дожидаемся завершения всех потоков в пуле
		for (auto& thread : _threads) {
			if (thread.joinable()) thread.join();
		}
	}

	//@brief Многопоточная обработка задач из очереди в пуле потоков
	void WorkerThread() {
		while (true) {
			std::function<void()> task;
			{
				// Захват мьютекса для безопасного доступа к очереди задач
				std::unique_lock<std::mutex> lock(_taskMutex);
				// Ожидаем, пока не появится задача в очереди или не придет сигнал остановки
				_taskCV.wait(lock, [this] { return !_tasks.empty() || _stop; });
				// Если пришел сигнал остановки и очередь задач пуста, завершаем выполнение потока
				if (_stop && _tasks.empty()) return;
				// Получение задачу из очереди
				task = std::move(_tasks.front()); 
				// Удаление задачи из очереди
				_tasks.pop();
			}
			// Выполнение задачи
			task(); 
		}
	}

	// @brief Добавление задачи в очередь
	void EnqueueTask(std::function<void()>&& task) {
		{
			// Захват мьютекса для безопасного добавления задачи в очередь
			std::lock_guard<std::mutex> lock(_taskMutex);
			// Уведомляем один из потоков о наличии новой задачи
			_tasks.emplace(std::move(task));
		}
		// Уведомляем один из потоков о наличии новой задачи
		_taskCV.notify_one();
	}
};

///@brief Перегрузка оператора вывода для класса Directory
std::ostream& operator<<(std::ostream& os, const Directory& directory) {
	// Выводим все поддиректории
	for (const auto& subdir : directory._directories) {
		os << "\t" << subdir.GetPath().string().substr(directory.GetPath().string().size()) <<
			" (Thread ID: " << subdir._threadId << ")\n";
		os << subdir;
	}

	// Выводим все файлы в текущей директории
	for (const auto& file : directory._filenames) {
		os << "\t\t" << file.string().substr(directory.GetPath().string().size()) <<
			" (Thread ID: " << directory._threadId << ")\n";
	}

	return os;
}

/// @brief Обход директории
static void TraverseDirectory(const std::filesystem::path& directory, ThreadPool& pool, std::chrono::milliseconds debugSleep) {
	Directory dir(directory);
	dir._threadId = std::this_thread::get_id();

	// Обходим все файлы и поддиректории в текущей директории
	for (const auto& file : std::filesystem::directory_iterator(directory)) {
		if (file.is_regular_file()) { 
			// Если это файл
			dir.AddFile(file.path());
		}
		else if (file.is_directory()) { 
			// Если это поддиректория
			if (debugSleep.count() > 0) {
				std::this_thread::sleep_for(debugSleep);
			}
			std::filesystem::path dirPath = file.path();
			dir.AddDirectory(dirPath);
			// Добавляем задачу в очередь для обработки этой поддиректории
			pool.EnqueueTask([&pool, dirPath, debugSleep]() {
				// Рекурсивный вызов для обхода поддиректории
				TraverseDirectory(dirPath, pool, debugSleep);
			});
		}
	}
	{
		// Захват мьютекса для безопасного вывода
		std::lock_guard<std::mutex> lock(pool._poolMutex);
		// Вывод информации о директории
		std::cout << dir;
	}
}


int main(int argc, const char** argv) {
	args_parse::ArgsParser parser(argc, argv);
	args_parse::Argument<bool> help('h', "help", false);
	help.SetDescription("Outputs a description of all added command line arguments");
	args_parse::Argument<unsigned int> thread_pool('t', "thread-pool", true, new args_parse::Validator<unsigned int>());
	thread_pool.SetDescription("Sets the number of threads (number)");
	args_parse::Argument<std::chrono::milliseconds> debug_sleep(
		'd', "debug-sleep", true, new args_parse::Validator<std::chrono::milliseconds>());
	debug_sleep.SetDescription("Input of the debug sleep thread (ms/s)");
	args_parse::Argument<std::string> source_path('s', "source-path", true, new args_parse::Validator<std::string>());
	source_path.SetDescription("Enter the directory path (without any delimiter/=) (path)");

	parser.Add(&help);
	parser.Add(&thread_pool);
	parser.Add(&debug_sleep);
	parser.Add(&source_path);

	if (parser.Parse()) {
		if (help.GetIsDefined()) {
			parser.ShowHelp();
		}
		if (source_path.GetIsDefined()) {

			std::filesystem::path sourcePath = source_path.GetValue().value();
			unsigned int threadPool = thread_pool.GetIsDefined() ? thread_pool.GetValue().value() : 0;
			std::chrono::milliseconds debugSleep = debug_sleep.GetIsDefined() ? debug_sleep.GetValue() : std::chrono::milliseconds(0);
			ThreadPool pool(threadPool, debugSleep);

			TraverseDirectory(sourcePath, pool, debugSleep);
		}
	}
	return 0;
}