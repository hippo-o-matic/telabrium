#include "luarium/level.h"

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

    std::vector<micropak::meta_entry> tags = micropak::unpack(path, LUARIUM_CACHE_PATH + "mapdata/"); // Unpack the map into the cache
    
    std::string index_path = (*std::find_if(tags.begin(), tags.end(), find_tag("index_level"))).value; // Look at the tags for an index map override
    if(index_path.empty())
        index_path = "index.json"; // If there is no override, use index.json as the default
  
    if(!fsys::exists(path + index_path)) { // Make sure the file exists
        index_level = ObjFactory::createObject("Level");
        index_level->to<Level>()->load(path + index_path);
    } else {
        LuariumLog("Could not find the index level, can't load map" + path, 2);
    }
    
}

Level::Level() {}

void Level::load(std::string path) {
    std::ifstream file(path.c_str());
    if(!file) {
        LuariumLog("File \"" + path + "\" was not found", 3);
        return;
    }

    Json::Value json;
    try { file >> json; } catch(const Json::RuntimeError& e) {
        LuariumLog(std::string("Json Error: ") + e.what(), 3);
        return;
    }
    

    if(json["type"] != "Level") {
        LuariumLog("Could not load the level: root object isn't type 'level'", 2);
        return;
    }

    jload(json);
}

void Level::jload(Json::Value lvl) {
    this->Object::jload(lvl);
}

void Level::unload() {
    components.clear();
}