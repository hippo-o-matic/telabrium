#ifndef LUARIUM_LEVEL_H
#define LUARIUM_LEVEL_H

#include <string>
#include <vector>
#include <fstream>

#include <experimental/filesystem>

#include "json/json.h"
#include "json/json-forwards.h"

#include "luarium/object.h"
#include "luarium/utility.h"
#include "micropak.hpp"

namespace Map {
	namespace fsys = std::experimental::filesystem; // That's a long name

	int load(std::string path, std::string options = ""); // Load the map 

	void unload(); // Cleans up resources of currently loaded map
	void clear_cache();
	void clear_cache(std::string path); // Clears specified entry from cache, if blank clears whole cache

	void cleanup(std::string options = ""); // Cleanup function for Luarium::cleanup()

	std::vector<std::string> map_cache;

	std::unique_ptr<Level> index;
};

class Level : public Object {
public:
	Level(); // Get the parent map path and the path inside the map file
	~Level();

	void load();
	void unload();
};

#endif