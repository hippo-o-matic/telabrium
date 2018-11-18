#ifndef LUARIUM_DEBUG_H
#define LUARIUM_DEBUG_H

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

namespace Luarium{
	#define logf(message, severity, outputfile) _filelog(message, severity, outputfile, __FILE__, __LINE__)
	void _filelog(std::string message, int severity, std::string outputfile, const char* filename, int line);

	#define log(message, severity) _log(message, severity, __FILE__, __LINE__)
	void _log(std::string message, int severity, const char* filename, int line);
}

#endif