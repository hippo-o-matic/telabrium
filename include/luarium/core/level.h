#ifndef LUARIUM_LEVEL_H
#define LUARIUM_LEVEL_H

#include <string>
#include <vector>

#include <experimental/filesystem>
#include <archive.h>

#include "luarium/core/object.h"
#include "luarium/core/generic.h"

namespace Map {

	namespace fsys = std::experimental::filesystem; // Holy fuck that's a long name

	void init(); //Initialize archive library12a''2'''''''';;''''''''''''''';;;;;;;;

	void load(std::string path, std::string options = ""); // Load the map 

	void pack(std::string dirPath, std::string out); // Compresses a map directory into a map file
	void unpack_to(std::string path, std::string out);
	void unpack_to_cache(std::string path = current_path); // Extract map to cache beforehand, leave blank for current map

	void unload(); // Cleans up resources of currently loaded map
	void clear_cache();
	void clear_cache(std::string path); // Clears specified entry from cache, if blank clears whole cache

	void cleanup(std::string options = ""); // Cleanup function for Luarium::cleanup()

	std::string current_path;

	archive* read; // The archive structs for reading and writing data
	archive* write;
};

class Level : public Object {
public:
	Level(std::string mapPath, std::string path); // Get the parent map path and the path inside the map file
	~Level();

	void load();
	void unload();

	std::string mapPath;
	std::string path;
};

#endif