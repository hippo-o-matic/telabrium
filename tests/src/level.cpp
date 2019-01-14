#include "luarium/core/level.h"

void Map::init() {
	read = archive_read_new();
	write = archive_write_new();

	archive_read_support_filter_gzip(read);
	archive_read_support_format_tar(read);
	archive_write_add_filter_gzip(write);
	archive_write_set_format_pax(write);

	if(!fsys::exists(LUARIUM_CACHE_PATH + "map/")){
		if(!fsys::exists(LUARIUM_CACHE_PATH)){
			fsys::create_directory(LUARIUM_CACHE_PATH);
		}
		fsys::create_directory(LUARIUM_CACHE_PATH + "map/");
	}
}

void Map::load(std::string path, std::string options){
//	std::string mapName = LUARIUM_CACHE_PATH + "map/" + Luarium::segment(path, ['/', '\\']).back;
	if(options.find("RELOAD_CACHE")) { // Ignore the cache version and reextract

	}
	if(options.find("DIRECT_LOAD")) { // Don't extract to cache, instead extract when the files are needed

	}
	if(options.find("PRELOAD")) { // Load the level in the background

	}
	if(options.find("LOAD_TO_CACHE")) { // Just extract the level

	}
	else {
//		if(fsys::exists(mapName)){

//		}
	}
}

void Map::unpack_to_cache(std::string path) {
	archive_entry* entry;

	archive_read_open_filename(read, path.c_str(), 10240);
	std::string outDir = LUARIUM_CACHE_PATH + "map/" + Luarium::segment(path, '/').back;
	archive_write_open_filename(write, outDir.c_str());
	archive_read_disk_set_standard_lookup(read);

	while (archive_read_next_header(read, &entry) == ARCHIVE_OK){
		archive_read_extract2(read, entry, write);
		archive_read_data_skip(read);
	}

	archive_read_close(read);
	archive_write_close(write);
}

void Map::clear_cache(){
	fsys::path rmv = LUARIUM_CACHE_PATH + "map/";
	fsys::remove_all(rmv);
}

void Map::clear_cache(std::string path){
	std::string pathname = Luarium::segment(path, '/').back;
	fsys::path rmv = LUARIUM_CACHE_PATH + "map/" + pathname;
	fsys::remove_all(rmv);
}

void Map::cleanup(std::string options){
	archive_read_free(read);
	archive_write_free(write);
	if(!options.find("KEEP_MAPS_ON_CLEANUP")){
		Map::clear_cache();
	}
}