#include "luarium/core/luafile.h"

LuaFile::LuaFile(std::string path){
	L = luaL_newstate(); //Create a new Luastate
	if (luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0)) {
		std::cout<<"[!] LUA|6: Lua Error: script not loaded ("<<path<<")"<<std::endl;
		lua_close(L);
		L = nullptr;
	}
}
LuaFile::~LuaFile(){
	if (L) lua_close(L); //Close the luafile after all refrences are removed
}

void LuaFile::printError(std::string name, std::string reason) {
	std::cout<<"[!] LUA|15: Lua Error: couldn't get ["<<(name)<<"]: "<<reason<<std::endl;
}

void LuaFile::clean(){
	if(L) lua_pop(L, lua_gettop(L)); //Removes all items from the lua stack
}

void LuaFile::getNamespace(std::string name){ 
		if(name !="global")
			nspace = name + ".";
		else
			nspace.clear();
	}

bool LuaFile::gettostack(std::string name){
	int lev = 0; //Our level in the stack
	std::string var = "";
	for (unsigned int i = 0; i < (nspace + name).size(); i++){
		if((nspace + name).at(i) == '.'){
			if(lev == 0){
				if(L) lua_getglobal(L, var.c_str());
			} else {
				if(L) lua_getfield(L, -1, var.c_str());
			}

			if (lua_isnil(L, -1)){
				printError((nspace + name), var + " is not defined");
				return false;
			} else {
				var = "";
				lev++;
			}
		} else {
			var += (nspace + name).at(i);
		}
	}

	if(lev == 0){
		if(L) lua_getglobal(L, var.c_str());
	} else{
		if(L) lua_getfield(L, -1, var.c_str());
	}

	if(lua_isnil(L, -1)){
		printError((nspace + name), var + " is not defined");
		return false;
	}

	return true;
}

/*std::vector<std::string> LuaFile::getKeys(){

}*/


void LuaFile::push(bool value){
	if(L) lua_pushboolean(L, value);
}
void LuaFile::push(int value){
	if(L) lua_pushinteger(L, value);
}
void LuaFile::push(float value){
	if(L) lua_pushnumber(L, value);
}
void LuaFile::push(std::string value){
	if(L) lua_pushstring(L, value.c_str());
}

/*void LuaFile::parse(std::string name, ...){
	va_list ap;
	int total = ;
	va_start(ap, total);
	lua_getglobal(L, name.c_str());
	for(int i = 0; i < total; i++){
		push(va_arg(ap, ))
	}
	va_end(ap);



	clean();
}*/