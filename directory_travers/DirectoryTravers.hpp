#pragma once
#include <thread>
#include <vector>
#include <filesystem>
#include <chrono>
#include <queue>
#include <mutex>
#include <functional>

namespace directory_travers {
	struct Directory {
		///id ������, ��������������� ������� ����������
		std::thread::id _threadId;
		///���� ����������
		std::filesystem::path _path;
		///������ ���� ������ ������� ����������
		std::vector<std::filesystem::path> _filenames;
		///������ ���� ������������� � ����������
		std::vector<Directory> _directories;

		Directory(const std::filesystem::path path) : _path{ path } {}

		///@brief ��������� ����
		[[nodiscard]] const std::filesystem::path& GetPath() const { return _path; }

		///@brief ��������� ����
		void SetPath(const std::filesystem::path path) { _path = path; }

		///@brief ���������� ����� � ������ ���� ������
		void AddFile(const std::filesystem::path& file)
		{
			_filenames.push_back(file);
		}

		///@brief ���������� ����� � ������ ���� ����������
		void AddDirectory(const std::filesystem::path& file) {
			_directories.push_back(Directory(file));
		}
	};

	struct ThreadPool {
		/// ���������� �������
		unsigned int _threadPool;
		/// ���������� ��������� � �������������
		std::chrono::milliseconds _debugSleep;
		/// �������� ��� ����
		std::mutex _poolMutex;
		///�������� ���������� ��� ����
		std::condition_variable _poolCV;
		///�������
		std::queue<std::function<void()>> _tasks;
		/// �������� ��� �������
		std::mutex _taskMutex;
		///�������� ���������� ��� �������
		std::condition_variable _taskCV;
		///������ �������
		std::vector<std::thread> _threads;

		bool _stop;

		ThreadPool(unsigned int threadPool, std::chrono::milliseconds debugSleep) :
			_threadPool(threadPool), _debugSleep(std::move(debugSleep)) {}

		~ThreadPool() {
			for (auto& thread : _threads)
				if (thread.joinable()) thread.join();
		}

		void Start() {
			for (unsigned int i = 0; i < _threadPool; ++i) {
				_threads.emplace_back([this] {
					while (true) {
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(_taskMutex);
							_taskCV.wait(lock, [this] { return !_tasks.empty() || _stop; });
							if (_stop && _tasks.empty()) {
								return;
							}
							task = std::move(_tasks.front());
							_tasks.pop();
						}
						task();
						std::this_thread::sleep_for(_debugSleep);
					}
					});
			}
		}

		void Stop() {
			{
				std::unique_lock<std::mutex> lock(_taskMutex);
				_stop = true;
			}
			_taskCV.notify_all();
			for (auto& thread : _threads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
			_threads.clear();
		}

		void AddTask(std::function<void()> task) {
			std::unique_lock<std::mutex> lock(_taskMutex);
			_tasks.push(std::move(task));
			_taskCV.notify_one();
		}

		void TraverseDirectory(const std::filesystem::path& path, directory_travers::Directory& dir) {
			dir._threadId = std::this_thread::get_id();
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				if (entry.is_directory()) {
					std::filesystem::path subdir_path = entry.path();
					AddTask([this, &subdir_path, &dir] { TraverseDirectory(subdir_path, dir); });
					dir.AddDirectory(subdir_path);
				}
				else {
					dir.AddFile(entry.path());
				}
			}
		}
	};
}