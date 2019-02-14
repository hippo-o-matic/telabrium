#include "luarium/generic.h"

//Parses strings into a vector of strings seperated by the seperator character
std::vector<std::string> Luarium::segment(std::string &in, char seperator) {
	size_t pos = in.find(seperator); //Limits in to 512 characters, might want to change later
	size_t last = 0;
	std::vector<std::string> output;
	if (!in.empty()) do {
		pos = in.find(seperator, last + 1);
		output.push_back(in.substr(last, pos - last));
		last = pos;
	} while (pos != std::string::npos);
	return output;
}

std::vector<Vertex> Luarium::calcVertex(const std::vector<float> &verticies, const std::vector<float> &texcoords) {
	std::vector<Vertex> v;
	
	// Establish some empty Vertex structs
	for(long unsigned int i=0; i < verticies.size() / 3; i++){
		v.push_back(Vertex()); 
	}

	// Put all the vertex coords into the vertex vector
	for(long unsigned int i = 0, j = 0; i <= v.size(); i++, j+=3) {
		v[i].Position = glm::vec3(verticies[j], verticies[j+1], verticies[j+2]);
	}

	// Calculate vertex normals, 
	for(long unsigned int i=0, j=0; i <= v.size(); i++, j+=3){ //The i + 3 is to make sure we don't go past where there are values
		glm::vec3 U(v[j+1].Position - v[j].Position);
		glm::vec3 V(v[j+2].Position - v[j].Position);

		glm::vec3 N(
			U.y*V.z - U.z*V.y,
			U.z*V.x - U.x*V.z,
			U.x*V.y - U.y*V.x
		);
 
		v[i].Normal = N;
	}

	// Generate texcoords if there are none
	if (texcoords.empty()){
		for(long unsigned int i=0; i + 4 <= v.size(); i+=4){
			v[i].TexCoords = glm::vec2(0, 0);
			v[i+1].TexCoords = glm::vec2(1, 0);
			v[i+2].TexCoords = glm::vec2(1, 1);
			v[i+3].TexCoords = glm::vec2(0, 1);
		}
	} else {
		for(long unsigned int i=0; i < v.size(); i++){
			v[i].TexCoords = glm::vec2(texcoords[i], texcoords[i+1]);
		}
	}

	//Calculate the tangent and bitangents for each triangle
	for(long unsigned int i=0; i + 2 < v.size(); i+=3){
		glm::vec3 edge1 = v[i+1].Position - v[i].Position;
		glm::vec3 edge2 = v[i+2].Position - v[i].Position;

		glm::vec2 delta1 = v[i+1].TexCoords - v[i].TexCoords;
		glm::vec2 delta2 = v[i+2].TexCoords - v[i].TexCoords;

		// Scary math
		float f = 1.0f / (delta1.x * delta2.y - delta2.x * delta1.y);

		v[i].Tangent.x = f * (delta2.y * edge1.x - delta1.y * edge2.x);
		v[i].Tangent.y = f * (delta2.y * edge1.y - delta1.y * edge2.y);
		v[i].Tangent.z = f * (delta2.y * edge1.z - delta1.y * edge2.z);
		v[i].Tangent = glm::normalize(v[i].Tangent);
		v[i+1].Tangent = v[i].Tangent;
		v[i+2].Tangent = v[i].Tangent;

		v[i].Bitangent.x = f * (-delta2.x * edge1.x + delta1.x * edge2.x);
		v[i].Bitangent.y = f * (-delta2.x * edge1.y + delta1.x * edge2.y);
		v[i].Bitangent.z = f * (-delta2.x * edge1.z + delta1.x * edge2.z);
		v[i].Bitangent = glm::normalize(v[i].Bitangent);
		v[i+1].Bitangent = v[i].Bitangent;
		v[i+2].Bitangent = v[i].Bitangent;
	}
	return v;
}

void Luarium::log(std::string message, short int severity, const char* outputfile){
	std::fstream file(outputfile, std::fstream::out);

	char out[80];
	time_t now = time(0);
	strftime(out, sizeof(out), "%Y-%m-%d.%X", localtime(&now));

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

	file << "<" << out << "> " << sev << message << "\n";
	std::cout << "<" << out << "> " << sev << message << "\n";
	file.close();
}