#include "telabrium/utility.h"

//Parses strings into a vector of strings seperated by the seperator character
std::vector<std::string> Telabrium::segment(std::string &in, char seperator) {
	size_t last = 0;
	size_t pos = 0;
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

// Replaces all instances of <target> with <result> in <input&>
std::string Telabrium::replaceAll(std::string& input, const std::string& target, const std::string& result) {
	if(target.empty())
		return input;

	size_t pos = 0;
	while((pos = input.find(target, pos)) != std::string::npos) {
		input.replace(pos, result.length(), result);
		pos += result.length();
	}
	return input;
}


void Telabrium::log(std::string message, const char* f, int line, short int severity, const char* outputfile){
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
	std::cout << "[" << f << ", " << line << "] " << sev << message << "\n";
	file.close();
}

Json::Value Telabrium::getJsonFile(std::string path) {
	std::ifstream file(path.c_str());
	if(!file) {
		TelabriumLog("File \"" + path + "\" was not found", 3);
		return "";
	}

	Json::Value json;
	try { 
		file >> json; 
	} catch(const Json::RuntimeError& e) {
		TelabriumLog(std::string("Json Error: ") + e.what(), 3);
		return "";
	} catch(const Json::LogicError& e) {
		TelabriumLog(std::string("Json Error: ") + e.what(), 3);
		return "";
	}

	return json;
}