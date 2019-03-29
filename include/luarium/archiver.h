#ifndef LUARIUM_ARCHIVE
#define LUARIUM_ARCHIVE

#include <fstream>
#include <string>
#include <vector>
#include <experimental/filesystem> // Change this to <filesystem> when g++ decides it's good enough 

#include "gzip/compress.hpp"
#include "gzip/decompress.hpp"

#include "luarium/utility.h"

namespace Luarium{
	struct archive_entry {
		bool isDir; // Is the entry a directory or not?
		std::string path; // The path to the file 
		long unsigned size; // The size of the data
	};

	// A class that combines and seperates files to and from .hpak files
	namespace Archive {
		namespace fsys = std::experimental::filesystem;

		bool pack(std::string dirname, std::string outname = ""); // Pack a directory (dirname) into a single file (outname).hpak
		bool unpack(std::string filename, std::string outputdir = ""); // Unpack (filename).hpak into a directory (outputdir)

		bool compress(std::string dirname, std::string outname = ""); // Pack and gzip a directory
		bool decompress(std::string filename, std::string outputdir = ""); // Decompress and unpack a gzipped archive

		unsigned short getVersion(std::string filename);
		unsigned short getVersion(std::fstream &stream);

		const unsigned short max_version = 1;
	};
}

#endif