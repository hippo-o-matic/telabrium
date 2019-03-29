#include "luarium/archiver.h"

bool Luarium::Archive::pack(std::string dirname, std::string outname) {
	fsys::path inDir(dirname); // Make a filesystem path
	if(!fsys::exists(inDir)) {
		Luarium::log("ARCHIVE: Directory to pack doesn't exist", 1);
		return false;
	}
	if(outname == "") {
		outname = segment(dirname, '/').back() + ".hpak"; // Name the output file after the directory's name
	}
	// if(fsys::exists(outname)) {
	// 	Luarium::log("ARCHIVE: File already exists", 1);
	// 	return false;
	// }

	std::fstream result(outname.c_str(), std::ios::out | std::ios::binary); // Create the resulting file
	if(!result) {
		Luarium::log("ARCHIVE: Unable to write to \"" + outname + "\", do you have permissions?", 1);
		return false;
	}
	result.seekp(std::ios::beg);

	std::vector<archive_entry> entries; // Vector of entries, like a table of contents before the actual file data
	for(auto &p: fsys::recursive_directory_iterator(fsys::absolute(inDir))){ // Recursively find each item in the directory
		if(!fsys::is_directory(p.path())){ 
			std::fstream current_file(p.path().c_str(), std::ios::in | std::ios::binary);

			if(current_file) {
				char* data = new char('\0');
				current_file.read(data, fsys::file_size(p.path()));
				result.write(data, fsys::file_size(p.path())); // Put the filedata into the result file
				delete data;

				if(!result.bad()) { // Check for errors in combining the files
					std::string path = p.path().string(); // Just a string of the filepath we're working with
					entries.push_back(archive_entry {
						false,
						path.erase(0, path.find(dirname) + dirname.length() + 1), // The path we store should just be relative to the archive, not root (+1 gets rid of the slash)
						fsys::file_size(p.path())
					}); // Make an entry for the file

				} else {
					Luarium::log("ARCHIVE: Unable to append \"" + p.path().string() + "\" to target file", 1);
				}
				current_file.close();

			} else {
				Luarium::log("ARCHIVE: Unable to access file \"" + p.path().string() + "\", likely inadequate permissions", 1);
			}

		} else { // File turned out to be a directory
			std::string path = p.path().string();
			entries.push_back(archive_entry { 
				true,
				path.erase(0, path.find(dirname) + dirname.length() + 1),
				0
			});

		}
	}

	result.seekp(std::ios::beg);
	result.write(reinterpret_cast<const char*>(&max_version), sizeof(unsigned short)); // Write the version
	std::size_t entry_num = entries.size();
	result.write(reinterpret_cast<const char*>(&entry_num), sizeof(std::size_t)); // Write the amount of entries the archive has

	for(auto it = entries.begin(); it != entries.end(); it++) { // Store all entry metadata at the beginning of the archive
		result.write(reinterpret_cast<const char*>(&it->size), sizeof(long unsigned));
		result.write(reinterpret_cast<const char*>(&it->isDir), sizeof(bool));
		result.write(reinterpret_cast<const char*>(&it->path), sizeof(std::string));
	}

	result.close();
	return true;
}

bool Luarium::Archive::unpack(std::string filename, std::string outputdir){
	fsys::path inDir(filename);
	if(!fsys::exists(inDir)) {
		Luarium::log("ARCHIVE: File to unpack doesn't exist!", 1);
		return false;
	}

	std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
	unsigned int cursor = 0;

	unsigned short version = getVersion(file); // Read from the beginning of the file for the format version
	cursor += sizeof(unsigned short); // Move the cursor to the next data block


	if(version > max_version) { // Make sure we're capable of reading this format
		Luarium::log("ARCHIVE: Unable to read archive - format is a newer version than reader", 1);
		return false;
	}

	if(version == 1) {
		file.seekg(cursor);
		std::size_t entry_num = 0;
		file.read(reinterpret_cast<char*>(&entry_num), sizeof(std::size_t)); // Get the amount of entries
		cursor += sizeof(std::size_t);
		std::vector<archive_entry> entries(entry_num);

		// Get the entries from the archive header
		for(auto it = entries.begin(); it != entries.end(); it++) {
			file.seekg(cursor);
			file.read(reinterpret_cast<char*>(&it->size), sizeof(long unsigned));
			cursor += sizeof(long unsigned);

			file.seekg(cursor);
			file.read(reinterpret_cast<char*>(&it->isDir), sizeof(bool));
			cursor += sizeof(bool);

			file.seekg(cursor);
			file.read(reinterpret_cast<char*>(&it->path), sizeof(std::string));
			cursor += sizeof(std::string);
		}

		// Now start creating files and directories based off of this entry
		for(auto it = entries.begin(); it != entries.end(); it++) {
			if (it->isDir){ // Create a directory if the entry is one
				fsys::path path(outputdir + it->path);
				fsys::create_directories(path);
			} else {
				char* buffer = new char('\0');
				std::fstream output(outputdir + it->path, std::ios::out | std::ios::binary); // Create a file for output

				if(!output){
					Luarium::log("ARCHIVE: Unable to write file \"" + outputdir + it->path + "\", likely inadequate permissions", 1);
					return false;
				}

				file.seekg(cursor);
				file.read(buffer, it->size); // Put the data from the archive into the buffer
				cursor += it->size;

				output.seekg(std::ios::beg);
				output.write(buffer, sizeof(buffer)); // Put the buffer data into the file

				if(output.bad())
					Luarium::log("oof");

				output.close();
				delete buffer; // Clean up the buffer and output file
			}
		}
		file.close();
		return true;
	}
	
	file.close();
	return false; // The version didn't match any avalible extraction methods
}

bool Luarium::Archive::compress(std::string dirname, std::string outname){
	return true;
}

bool Luarium::Archive::decompress(std::string filename, std::string outputdir){
	return true;
}

unsigned short Luarium::Archive::getVersion(std::string filename) {
	unsigned short version = 0;

	fsys::path inDir(filename);
	if(!fsys::exists(inDir)) {
		Luarium::log("ARCHIVE: File to check doesn't exist!", 1);
		return false;
	}

	std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
	if(file){
		file.seekg(0); 
		file.read(reinterpret_cast<char*>(&version), sizeof(unsigned short));
	}

	return version;
}

unsigned short Luarium::Archive::getVersion(std::fstream &stream) {
	unsigned short version = 0;

	if(stream){
		stream.seekg(0); 
		stream.read(reinterpret_cast<char*>(&version), sizeof(unsigned short));
	}

	return version;
}