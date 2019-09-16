#include "luarium/utility.h"

//Parses strings into a vector of strings seperated by the seperator character
std::vector<std::string> Luarium::segment(std::string &in, char seperator) {
	size_t last = 0;
	size_t pos = 0; //Limits in to 512 characters, might want to change later
	std::vector<std::string> output;

	if (in.empty()) 
		return output;
	
	while (pos != std::string::npos) {
		pos = in.find(seperator, last);
		output.push_back(in.substr(last, pos - last));
		last = pos + 1;
	}

	return output;
}

void Luarium::log(std::string message, const char* f, int line, short int severity, const char* outputfile){
	std::fstream file(outputfile, std::fstream::out);

	char t[80];
	time_t now = time(0);
	strftime(t, sizeof(t), "%Y-%m-%d.%X", localtime(&now));

	std::string sev;
	switch(severity){
		case 1:
			sev = "[?] "; // Info or message
		case 2:
			sev = "[!] "; // Warning, couldn't find model or texture file
		case 3:
			sev = "[!!] "; // Error, a GL error occured
		case 4:
			sev = "[!!!] "; // Critical Error, the engine is going down 
		default:
			sev = "";
	}

	file << "<" << t << "> " << sev << message << "\n";
	std::cout << "<" << t << "> " << "[" << f << ", " << line << "] " << sev << message << "\n";
	file.close();
}