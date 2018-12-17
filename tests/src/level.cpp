#include "luarium/core/level.h"

void Map::init(){
	read = archive_read_new();
	write = archive_write_new();

	archive_read_support_filter_gzip(read);
	archive_read_support_format_tar(read);
	archive_write_add_filter_gzip(write);
	archive_write_set_format_pax(write);
}

void Map::load(std::string path, std::string options){
	Luarium::segment(options, ',');
}

void Map::unpack_to_cache(std::string path){
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
	
}

void Map::clear_cache(std::string path){
	
}

void Map::cleanup(){
	archive_read_free(read);
	archive_write_free(read);
}