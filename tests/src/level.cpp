#include "luarium/level.h"

struct find_tag {
    std::string y;
    find_tag(std::string y) : y(y) {}
    bool operator()(const micropak::meta_entry& m) const
    {
        return y == m.name;
    }
};

int Map::load(std::string path, std::string options) {
    if(!path.back() == '/' && !path.empty()) // Make sure the path is valid
        path += "/";

    std::vector<micropak::meta_entry> tags = micropak::unpack(path, LUARIUM_CACHE_PATH + "mapdata/"); // Unpack the map into the cache
    
    std::string index_path = (*std::find_if(tags.begin(), tags.end(), find_tag("index_level"))).value; // Look at the tags for an index map override
    if(index_path.empty())
        index_path = "index.json"; // If there is no override, use index.json as the default
    if(!fsys::exists(path + index_path)) { // Make sure the file exists
        index = std::dynamic_cast<std::unique_ptr<Level>>(create<Level>());
        index->load(path + index_path);
    } else {
        Luarium::log("Could not find the index level, can't load map" + path, 2);
    }
    
}

void Level::load() {
}