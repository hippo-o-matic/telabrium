#include "telabrium/level.h"

Object::ptr Map::index_level;

struct find_tag {
    std::string y;
    find_tag(std::string y) : y(y) {}
    bool operator()(const micropak::meta_entry& m) const
    {
        return y == m.name;
    }
};

void Map::load(std::string path, std::string options) {
    if(path.back() != '/' && !path.empty()) // Make sure the path is valid
        path += "/";

    std::vector<micropak::meta_entry> tags = micropak::unpack(path, TELABRIUM_CACHE_PATH + "mapdata/"); // Unpack the map into the cache
    
    std::string index_path = (*std::find_if(tags.begin(), tags.end(), find_tag("index_level"))).value; // Look at the tags for an index map override
    if(index_path.empty())
        index_path = "index.json"; // If there is no override, use index.json as the default
  
    if(!fsys::exists(path + index_path)) { // Make sure the file exists
        index_level = ObjFactory::createObject("Level", Level::getFileContents(path + index_path));
    } else {
        TelabriumLog("Could not find the index level, can't load map" + path, 2);
    }
    
}


Level::Level(Json::Value j) : Object3d(){
	try {
		// If a path is specified, go and get the level from that path
		if(j.get("path", "").asString() != "") {
			j = getFileContents(j["path"].asString());

			if(j.get("path", "").asString() != "") { // The level object redirected to can't be another redirect
				TelabriumLog("Level path cannot lead to another level path object", 3);
				return;
			}
		}
	} catch(const Json::LogicError& e) { // If it can't read the json object, it's possible <j> is just a string, so try to load it straight from that as a path
		// TODO: yeah this aint it chief
		std::string path = j.asString();
		j = getFileContents(path);
	}

	// Extract object traits without creating components yet
	this->id = j.get("id", "").asString();
	this->type = j.get("type", "Level").asString();

	Json::Value jPos = j["pos"];
	Json::Value jRot = j["rot"];
	Json::Value jScl = j["scl"];

	position = glm::vec3(jPos[0].asFloat(), jPos[1].asFloat(), jPos[2].asFloat());
	rotation = glm::vec3(jRot[0].asFloat(), jRot[1].asFloat(), jRot[2].asFloat());
	scale = glm::vec3(jScl[0].asFloat(), jScl[1].asFloat(), jScl[2].asFloat());

	contents = j["components"];
}

void Level::loadFile(std::string path) {
	contents = getFileContents(path)["components"];
	reload();
}

Json::Value Level::getFileContents(std::string path) {
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

	if(json["type"] != "Level") {
		TelabriumLog("Could not load the level: root object isn't type 'Level'", 2);
		return "";
	}

	return json;
}

void Level::reload() {
	unload();
	createComponents(contents);
}

void Level::unload() {
	for(auto&& it : components)
		std::remove(components.begin(), components.end(), it);
    components.clear();
}