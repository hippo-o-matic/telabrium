#include "hipponium/luafile.h"

LuaFile::LuaFile(std::string path){
	L = luaL_newstate(); //Create a new Luastate
	if (luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0)) {
		std::cout<<"Error: script not loaded ("<<path<<")"<<std::endl;
		L = 0;
	}
}
LuaFile::~LuaFile(){
	if (L) lua_close(L); //Close the luafile after all refrences are removed
}

void LuaFile::printError(std::string name, std::string reason) {
	std::cout<<"[!] LUA|24: Lua Error: couldn't get ["<<(nspace + "." + name)<<"]: "<<reason<<std::endl;
}

void LuaFile::clean(){
	lua_pop(L, lua_gettop(L)); //Removes all items from the lua stack
}

bool LuaFile::gettostack(std::string name){
	int lev = 0; //Our level in the stack
	std::string var = "";
	for (unsigned int i = 0; i < (nspace + "." + name).size(); i++){
		if((nspace + "." + name).at(i) == '.'){
			if(lev == 0){
				lua_getglobal(L, var.c_str());
			} else {
				lua_getfield(L, -1, var.c_str());
			}

			if (lua_isnil(L, -1)){
				printError((nspace + "." + name), var + " is not defined");
				return false;
			} else {
				var = "";
				lev++;
			}
		} else {
			var += (nspace + "." + name).at(i);
		}
	}

	if(lev == 0){
		lua_getglobal(L, var.c_str());
	} else{
		lua_getfield(L, -1, var.c_str());
	}

	if(lua_isnil(L, -1)){
		printError((nspace + "." + name), var + " is not defined");
		return false;
	}

	return true;
}

std::vector<std::string> LuaFile::getKeys(){

}

void LuaFile::parse(std::string name, ...){
	va_list ap;
	int total = ;
	va_start(ap, total);
	lua_getglobal(L, name.c_str());
	for(int i = 0; i < total; i++){
		push(va_arg(ap, ))
	}
	va_end(ap);



	clean();
}