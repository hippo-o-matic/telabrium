#include "luarium/archiver.h"

bool Luarium::micropak::pack(std::string dirname, std::string outname, bool compress, std::vector<Luarium::micropak::meta_entry> m_entries) {
	fsys::path inDir = dirname; // Make a filesystem path
	if(!fsys::exists(inDir)) {
		std::cout << "ARCHIVE: Directory to pack doesn't exist" << std::endl;
		return false;
	}
	if(outname == "") {
		outname = dirname.substr(dirname.find_last_of('/') + 1) + ".mpak"; // Name the output file after the directory's name
	}
	// if(fsys::exists(outname)) {
	// 	std::cout << "ARCHIVE: File already exists" << std::endl;
	// 	return false;
	// }

	std::fstream result(outname.c_str(), std::ios::out | std::ios::binary | std::ios::trunc); // Create the resulting file
	if(!result) {
		std::cout << "ARCHIVE: Unable to write \"" + outname + "\", do you have permissions?" << std::endl;
		return false;
	}	

	std::vector<archive_entry> entries; // Vector of entries, like a table of contents before the actual file data
	for(auto &p: fsys::recursive_directory_iterator(fsys::absolute(inDir))){ // Recursively find each item in the directory
		std::string path = p.path().string(); // A string with the path name
		path.erase(0, fsys::absolute(inDir).string().size() + 1); // The path we store should just be relative to the archive, not root (+1 gets rid of the slash)

		if(!fsys::is_directory(p.path())){ 
			entries.push_back(archive_entry {
				false,
				path,
				fsys::file_size(p.path())
			}); // Make an entry for the file

		} else { // File turned out to be a directory
			entries.push_back(archive_entry { 
				true,
				path,
				0
			});
		}
	}

	unsigned cursor = 0;

	result.seekp(std::ios::beg);
	result.write(reinterpret_cast<const char*>(&max_version), sizeof(unsigned short)); // Write the version
	cursor += sizeof(unsigned short);

	char* buffer = new char[LUARIUM_MAX_ARCHIVE_BUFFER_SIZE];
	for(auto it = entries.begin(); it != entries.end(); it++){ // Get the filedata for each entry and append it
		if(!it->isDir && fsys::exists(dirname + '/' + it->path)){ 
			std::fstream input_file(dirname + '/' + it->path, std::ios::in | std::ios::binary);
			input_file.seekg(std::ios::beg);

			if(input_file) {
				for(float i = 0; i < ((float)it->size/(float)LUARIUM_MAX_ARCHIVE_BUFFER_SIZE); i++) { // If the file is too big, seperate it into chunks
					if(i+1 > it->size/LUARIUM_MAX_ARCHIVE_BUFFER_SIZE) { // If the data left doesn't fill a chunk, be more precise
						input_file.read(buffer, it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i));

						if(compress) {
							std::string cbuffer = gzip::compress(buffer, it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i));
							result.seekp(cursor);
							result.write(cbuffer.c_str(), it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i)); // Put the filedata into the result file
							cursor += it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i);
						} else {
							result.seekp(cursor);
							result.write(buffer, it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i)); // Put the filedata into the result file
							cursor += it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i);
						}
					} else { // Otherwise just write in chunks of the max size
						input_file.read(buffer, LUARIUM_MAX_ARCHIVE_BUFFER_SIZE);

						if(compress) {
							std::string cbuffer = gzip::compress(buffer, LUARIUM_MAX_ARCHIVE_BUFFER_SIZE);
							result.seekp(cursor);
							result.write(cbuffer.c_str(), LUARIUM_MAX_ARCHIVE_BUFFER_SIZE);
							cursor += LUARIUM_MAX_ARCHIVE_BUFFER_SIZE;

							
						} else {
							result.seekp(cursor);
							result.write(buffer, LUARIUM_MAX_ARCHIVE_BUFFER_SIZE);
							cursor += LUARIUM_MAX_ARCHIVE_BUFFER_SIZE;
						}
					}
				}
			} else {
				std::cout << "ARCHIVE: Unable to append \"" + dirname + '/' + it->path + "\" to target file" << std::endl;
			}
		}
	}
	delete buffer;


	// Write the archive info as a footer
	//////////////////////////////////////
	size_t footer_start = cursor; // Remember where the footer starts

	result.seekp(cursor);
	std::size_t meta_num = m_entries.size();
	result.write(reinterpret_cast<const char*>(&meta_num), sizeof(std::size_t)); // Write the amount of meta tags the archive has
	cursor += sizeof(std::size_t);

	result.seekp(cursor);
	std::size_t entry_num = entries.size();
	result.write(reinterpret_cast<const char*>(&entry_num), sizeof(std::size_t)); // Write the amount of entries the archive has
	cursor += sizeof(std::size_t); 

	for(auto it = entries.begin(); it != entries.end(); it++) { // Store all entry data at the end of the archive, now that we know compressed file sizes
		result.seekp(cursor); // Seek to cursor position
		result.write(reinterpret_cast<const char*>(&it->isDir), sizeof(bool)); // Write the data value (in this case if the entry is a directory)
		cursor += sizeof(bool); // Advance cursor position

		size_t path_size = it->path.size() * sizeof(std::string::value_type); // Find the size of the path in bytes (currently each charachter in a string is 1 byte anyway, but this could change)
		result.seekp(cursor);
		result.write(reinterpret_cast<const char*>(&path_size), sizeof(size_t));
		cursor += sizeof(size_t);

		result.seekp(cursor);
		result.write(it->path.c_str(), path_size); // write the path relative to the root of the archive
		cursor += path_size;

		result.seekp(cursor);
		result.write(reinterpret_cast<const char*>(&it->size), sizeof(long unsigned)); // Write the size in bytes of the entry (0 if a directory)
		cursor += sizeof(long unsigned);
	}

	for(auto it = m_entries.begin(); it != m_entries.end(); it++){
		result.seekp(cursor);
		size_t name_size = it->name.size() * sizeof(std::string::value_type);
		result.write(reinterpret_cast<const char*>(it->name.size() * sizeof(std::string::value_type)), sizeof(size_t)); // Write the size of the name string in bytes
		cursor += sizeof(size_t);

		result.seekp(cursor);
		result.write(it->name.c_str(), name_size); // Write the content of the tag
		cursor += name_size;

		result.seekp(cursor);
		size_t value_size = it->value.size() * sizeof(std::string::value_type);
		result.write(reinterpret_cast<const char*>(&value_size), sizeof(size_t)); // Write the size of the value string in bytes
		cursor += sizeof(size_t);

		result.seekp(cursor);
		result.write(it->value.c_str(), value_size); 
		cursor += value_size;
	}

	// Finally, write the location that the footer starts
	result.seekp(cursor);
	result.write(reinterpret_cast<const char*>(footer_start), sizeof(size_t));

	result.close();
	return true;
}



std::vector<Luarium::micropak::meta_entry> Luarium::micropak::unpack(std::string filename, std::string outputdir){
	fsys::path inDir = filename;
	if(!fsys::exists(inDir)) {
		std::cout << "ARCHIVE: File to unpack doesn't exist!" << std::endl;
		return {};
	}

	if(!(outputdir.back() == '/') && !outputdir.empty()) // Format the path correctly, add a slash at the end
		outputdir += '/';

	std::fstream input_file(filename.c_str(), std::ios::in | std::ios::binary);
	unsigned int cursor = 0;

	unsigned short version = getVersion(input_file); // Read from the beginning of the file for the format version
	cursor += sizeof(unsigned short); // Move the cursor to the next data block


	if(version > max_version) { // Make sure we're capable of reading this format
		std::cout << "ARCHIVE: Unable to read archive - format is a newer version than reader" << std::endl;
		return {};
	}

	//EXTRACTION METHOD v1
	//////////////////////
	if(version == 1) {
		fsys::create_directories(outputdir); // Make the output path if it doesn't exist yet

		// Find and seek to the start of the footer
		input_file.seekg(std::ios::end - sizeof(size_t));
		input_file.read(reinterpret_cast<char*>(cursor), sizeof(size_t)); 

		input_file.seekg(cursor);
		size_t meta_num = 0;
		input_file.read(reinterpret_cast<char*>(&meta_num), sizeof(std::size_t)); // Get the amount of meta entries
		cursor += sizeof(std::size_t);

		input_file.seekg(cursor);
		size_t entry_num = 0;
		input_file.read(reinterpret_cast<char*>(&entry_num), sizeof(std::size_t)); // Get the amount of entries
		cursor += sizeof(std::size_t);

		std::vector<meta_entry> m_entries(meta_num);
		std::vector<archive_entry> entries(entry_num);

		for(auto it = m_entries.begin(); it != m_entries.end(); it++){ // Extract the meta tags
			input_file.seekg(cursor);
			size_t name_size = it->name.size() * sizeof(std::string::value_type);
			input_file.read(reinterpret_cast<char*>(it->name.size() * sizeof(std::string::value_type)), sizeof(size_t)); // Get the size of the name string
			cursor += sizeof(size_t);

			char* name_extract = new char[name_size+1]; // Create a buffer for the char data
			input_file.seekg(cursor);
			input_file.read(name_extract, name_size); // Read data into buffer
			cursor += name_size;
			name_extract[name_size] = '\0'; // Null-terminate the buffer data
			it->name = name_extract; // Assign the buffer data to storage

			// Do the same for the value assigned to the name
			input_file.seekg(cursor); 
			size_t value_size = it->value.size() * sizeof(std::string::value_type);
			input_file.write(reinterpret_cast<char*>(&value_size), sizeof(size_t));
			cursor += sizeof(size_t);

			char* value_extract = new char[value_size+1];
			input_file.seekg(cursor);
			input_file.read(value_extract, value_size); 
			cursor += name_size;
			value_extract[value_size] = '\0'; 
			it->value = value_extract; 

			delete name_extract;
			delete value_extract;
		}

		// Get the entries from the archive header
		for(auto it = entries.begin(); it != entries.end(); it++) {
			input_file.seekg(cursor);
			input_file.read(reinterpret_cast<char*>(&it->isDir), sizeof(bool));
			cursor += sizeof(bool);

			size_t path_size = 0;
			input_file.seekg(cursor);
			input_file.read(reinterpret_cast<char*>(&path_size), sizeof(size_t));
			cursor += sizeof(size_t);
			
			char* extract = new char[path_size+1];

			input_file.seekg(cursor);
			input_file.read(extract, path_size);
			cursor += path_size;

			extract[path_size] = '\0';
			it->path = extract;
			delete extract;

			input_file.seekg(cursor);
			input_file.read(reinterpret_cast<char*>(&it->size), sizeof(long unsigned));
			cursor += sizeof(long unsigned);
		}

		// Now start creating files and directories based off of this entry
		cursor = sizeof(unsigned short); // Go to where data starts

		char* buffer = new char[LUARIUM_MAX_ARCHIVE_BUFFER_SIZE];
		for(auto it = entries.begin(); it != entries.end(); it++) {
			fsys::path path = outputdir + it->path;
			path.make_preferred();
			if (it->isDir){ // Create a directory if the entry is one
				fsys::create_directories(path);
			} else {
				std::fstream output_file(path.c_str(), std::ios::out | std::ios::binary);
				output_file.seekp(std::ios::beg);
				unsigned output_cursor = 0;

				if(output_file) {
					// If the file is too big, seperate it into chunks
					for(unsigned i = 0; i < (float)it->size/(float)LUARIUM_MAX_ARCHIVE_BUFFER_SIZE; i++) {
						if(i+1 > it->size/LUARIUM_MAX_ARCHIVE_BUFFER_SIZE) { // If the data left doesn't fill a chunk, be more precise
							input_file.seekg(cursor);
							input_file.read(buffer, it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i));
							cursor += it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i);

							output_file.seekp(output_cursor);
							output_file.write(buffer, it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i)); // Put the filedata into the result file
							output_cursor += it->size - (LUARIUM_MAX_ARCHIVE_BUFFER_SIZE * i);
							*buffer = '\0'; // Remove the data from the buffer, but don't deallocate yet
						} else { // Otherwise just write in chunks of the max size
							input_file.seekg(cursor);
							input_file.read(buffer, LUARIUM_MAX_ARCHIVE_BUFFER_SIZE);
							cursor += it->size;

							output_file.seekp(output_cursor);
							output_file.write(buffer, LUARIUM_MAX_ARCHIVE_BUFFER_SIZE);
							output_cursor += LUARIUM_MAX_ARCHIVE_BUFFER_SIZE;
							*buffer = '\0';
						}
					}
				} else {
					std::cout << "ARCHIVE: Unable to create file \"" + path.string() + "\"" << std::endl;
				}
			}
		}
		delete buffer;
		return m_entries;
	}
	
	input_file.close();
	std::cout << "ARCHIVE: The archive didn't match any avalible extraction methods" << std::endl;
	return {}; // The version didn't match any avalible extraction methods
}

unsigned short Luarium::micropak::getVersion(std::string filename) {
	unsigned short version = 0;

	fsys::path inDir = filename;
	if(!fsys::exists(inDir)) {
		std::cout << "ARCHIVE: File to check doesn't exist!" << std::endl;
		return false;
	}

	std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
	if(file){
		file.seekg(0); 
		file.read(reinterpret_cast<char*>(&version), sizeof(unsigned short));
	}

	return version;
}

unsigned short Luarium::micropak::getVersion(std::fstream &stream) {
	unsigned short version = 0;

	if(stream){
		stream.seekg(0); 
		stream.read(reinterpret_cast<char*>(&version), sizeof(unsigned short));
	}

	return version;
}