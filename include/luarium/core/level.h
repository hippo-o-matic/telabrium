#ifndef LUARIUM_LEVEL_H
#define LUARIUM_LEVEL_H

#include <string>
#include <vector>
#include <filesystem>
#include <archive.h>

#include "luarium/core/object.h"
#include "luarium/core/generic.h"

const std::string LUARIUM_CACHE_PATH = "cache/";

namespace Map {
	void init(); //Initialize archive library

	void load(std::string path, std::string options); // Load the map 
	void load_force_cache(std::string path); // Load the map, ignoring the current cache
	void load_without_cache(std::string path); // Load the currently initialized map directly from the file, without extracting

	void pack(std::string dirPath, std::string out); // Compresses a map directory into a map file
	void unpack_to(std::string path, std::string out);
	void unpack_to_cache(std::string path = current_path); // Extract map to cache beforehand, leave blank for current map

	void unload(); // Cleans up resources of currently loaded map
	void clear_cache();
	void clear_cache(std::string path); // Clears specified entry from cache, if blank clears whole cache

	void cleanup(); // Cleanup function for Luarium::cleanup()

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