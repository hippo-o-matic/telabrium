#ifndef LUARIUM_LEVEL_H
#define LUARIUM_LEVEL_H

#include <string>
#include <vector>

#include <experimental/filesystem>
#include <archive.h>

#include "luarium/object.h"
#include "luarium/utility.h"
#include "luarium/archiver.h"

namespace Map {

	namespace fsys = std::experimental::filesystem; // That's a long name

	void init(); //Initialize archive library

	void load(std::string path, std::string options = ""); // Load the map 

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

class SetReader {
	SetReader();
	
}

#endif