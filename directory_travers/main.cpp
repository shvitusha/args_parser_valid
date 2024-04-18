#include "args_parse/argument.hpp"
#include "args_parse/ArgsParser.hpp"
#include "DirectoryTravers.hpp"

int main(int argc, const char** argv) {
	args_parse::ArgsParser parser(argc, argv);
	args_parse::Argument<bool> help('h', "help", false);
	help.SetDescription("Outputs a description of all added command line arguments");
	args_parse::Argument<unsigned int> thread_pool('t', "thread-pool", true, new args_parse::Validator<unsigned int>());
	thread_pool.SetDescription("Sets the number of threads (number)");
	args_parse::Argument<std::chrono::milliseconds> debug_sleep('d', "debug-sleep", true, new args_parse::Validator<std::chrono::milliseconds>());
	args_parse::Argument<std::string> source_path('s', "source-path", true, new args_parse::Validator<std::string>());

	parser.Add(&help);
	parser.Add(&thread_pool);
	parser.Add(&debug_sleep);
	parser.Add(&source_path);

	/*if (parser.Parse()) {
		if (help.GetIsDefined()) {
			parser.ShowHelp();
		}
	}*/

	if (parser.Parse()) {
		if (help.GetIsDefined()) {
			parser.ShowHelp();
		}
		if (source_path.GetIsDefined()) {
			std::filesystem::path path(source_path.GetValue().value());
			if (!std::filesystem::exists(path)) {
				std::cerr << "Source path does not exist: " << path << std::endl;
				return 1;
			}
			if (!std::filesystem::is_directory(path)) {
				std::cerr << "Source path is not a directory: " << path << std::endl;
				return 1;
			}

			directory_travers::ThreadPool pool(thread_pool.GetValue().value(), debug_sleep.GetValue());
			pool.Start();

			directory_travers::Directory root(path);
			pool.AddTask([&pool, path, &root] { pool.TraverseDirectory(path, root); }); // Передаем путь к корневой директории

			pool.Stop();

			for (const auto& dir : root._directories) {
				std::cout << "Directory: " << dir._path << " (Thread ID: " << dir._threadId << ")" << std::endl;
				for (const auto& file : dir._filenames) {
					std::cout << "    " << file << std::endl;
				}
			}
		}
	}

	return 0;
}