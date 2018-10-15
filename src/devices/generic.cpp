#include "luarium/generic.h"

std::vector<std::string> Luarium::segment(std::string &in, char seperator) {
	size_t pos = in.find(seperator); //Limits commands to 512 characters, might want to change later
	size_t last = 0;
	std::vector<std::string> output;
	if (!in.empty()) do {
		pos = in.find(seperator, last);
		output.push_back(in.substr(last, pos - last));
		last = pos;
	} while (pos != std::string::npos);
	return output;
}