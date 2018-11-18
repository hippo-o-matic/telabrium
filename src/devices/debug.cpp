#include "luarium/debug.h"

void Luarium::_filelog(std::string message, int severity, std::string outputfile, const char* filename, int line){
	std::fstream file(outputfile);
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	file << "<" << now << "> " << filename << "|" << line << ": " << message << "\n";
	file.close();
}

void Luarium::_log(std::string message, int severity, const char* filename, int line){
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::cout << "<" << now << "> " << filename << "|" << line << ": " << message << "\n";
}