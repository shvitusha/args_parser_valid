#include <args_parse/argument.hpp>
#include <args_parse/ArgsParser.hpp>
#include <chrono>
#include <iostream>

int main(int argc, const char** argv)
{
	args_parse::ArgsParser parser(argc, argv);

	args_parse::Argument<bool> help('h', "help", false);
	help.SetDescription("Outputs a description of all added command line arguments");
	args_parse::Argument<bool> verbose('v', "verbose", false);
	verbose.SetDescription("Outputs a verbose of all added command line arguments");
	args_parse::Argument<std::string> input('i', "input", true, new args_parse::Validator<std::string>());
	input.SetDescription("Input (filename)");
	args_parse::Argument<std::string> output('o', "output", true, new args_parse::Validator<std::string>());
	output.SetDescription("Output (filename)");
	args_parse::Argument<int> number('n', "number", true, new args_parse::Validator<int>());

	args_parse::Argument<float> parametr('p', "parametr", true, new args_parse::Validator<float>());
	parametr.SetDescription("Definition of constant/precision/parameter with floating sign");
	args_parse::Argument<unsigned int> thread_pool('t', "thread-pool", true, new args_parse::Validator<unsigned int>());
	thread_pool.SetDescription("Sets the number of threads (number)");
	args_parse::Argument<std::chrono::milliseconds> debug_sleep('d', "debug-sleep", true, new args_parse::Validator<std::chrono::milliseconds>());
	debug_sleep.SetDescription("Defines a user input of the argument type (ms/s)");

	parser.Add(&help);
	parser.Add(&verbose);
	parser.Add(&input);
	parser.Add(&output);
	parser.Add(&number);
	parser.Add(&parametr);
	parser.Add(&thread_pool);
	parser.Add(&debug_sleep);

	if (parser.Parse()) {
		if (help.GetIsDefined()) {
			parser.ShowHelp();
		}
		if (verbose.GetIsDefined()) {
			parser.ShowHelpVerbose();
		}
		if (output.GetIsDefined()) {
			std::cout << "Output o value: " << output.GetValue().value() << std::endl;
		}
		if (number.GetIsDefined()) {
			std::cout << "Input n value: " << number.GetValue().value() << std::endl;
		}
		if (input.GetIsDefined()) {
			std::cout << "Input i value: " << input.GetValue().value() << std::endl;
		}
		if (thread_pool.GetIsDefined()) {
			std::cout << "Input t value: " << thread_pool.GetValue().value() << std::endl;
		}
		if (debug_sleep.GetIsDefined()) {
			std::cout << "Input d value (ms/s): " << debug_sleep.GetValue().count() << std::endl;
		}
		if (parametr.GetIsDefined()) {
			std::cout << "Input p value (float): " << parametr.GetValue().value() << std::endl;
		}
	}
	return 0;
}