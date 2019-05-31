#ifndef LUARIUM_ARCHIVE
#define LUARIUM_ARCHIVE

#define LUARIUM_MAX_ARCHIVE_BUFFER_SIZE 5000000 //The maximum amount of memory to take for file extraction/compression (default 5 MB)

#include <fstream>
#include <string>
#include <vector>
#include <experimental/filesystem> // Change this to <filesystem> when g++ decides it's good enough 

#include "gzip/compress.hpp"
#include "gzip/decompress.hpp"

#include "luarium/utility.h"

namespace Luarium{
	// A class that combines and seperates files to and from .mpak files
	namespace micropak {
		namespace fsys = std::experimental::filesystem;

		struct archive_entry {
			bool isDir; // Is the entry a directory or not?
			std::string path; // The path to the file 
			long unsigned size; // The size of the data
		};

		struct meta_entry { // Additional data stored in the file header
			std::string name;
			std::string value;
		};

		bool pack(std::string dirname, std::string outname = "", bool compress = true, std::vector<micropak::meta_entry> m_entries = {}); // Pack a directory (dirname) into a single file (outname).mpak
		std::vector<micropak::meta_entry> unpack(std::string filename, std::string outputdir = "."); // Unpack (filename).mpak into a directory (outputdir)

		unsigned short getVersion(std::string filename);
		unsigned short getVersion(std::fstream &stream);

		const unsigned short max_version = 1;
	};
}

#endif