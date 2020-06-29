#ifndef TELABRIUM_LEVEL_H
#define TELABRIUM_LEVEL_H

#include <string>
#include <vector>
#include <fstream>

#include <experimental/filesystem>

#include "json/json.h"

#include "telabrium/object3d.hpp"
#include "telabrium/utility.h"
#include "micropak.hpp"

namespace Map {
	namespace fsys = std::experimental::filesystem; // That's a long name

	void load(std::string path, std::string options = ""); // Load the map 

	void unload(); // Cleans up resources of currently loaded map
	void clear_cache();
	void clear_cache(std::string path); // Clears specified entry from cache, if blank clears whole cache

	void cleanup(std::string options = ""); // Cleanup function for Telabrium::cleanup()

	extern std::vector<std::string> map_cache;

	extern Object::ptr index_level;
};

class Level : public Object3d {
public:
	// Level(std::string); // Get the parent map path and the path inside the map file
	Level(Json::Value);
	~Level() = default;

	static Json::Value getFileContents(std::string path);
	void loadFile(std::string path); // Gets the data from the specified path
	void reload(); // Loads the level with the data already in this->contents
	void unload();

private:
	Json::Value contents;
};

#endif