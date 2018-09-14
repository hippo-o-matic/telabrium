#ifndef HIPPO_LUAFILE_H
#define HIPPO_LUAFILE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory> //smart pointers I guess
#include <cstdarg> //For parsing functions with paramaters

#include "lua/lua.hpp"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

class LuaFile{
public:
	LuaFile(std::string path);
	~LuaFile();

	void printError(std::string name, std::string reason);

	void clean();


	template <typename T>
	T get(std::string name);

	template<typename T>
	std::vector<T> getVector(std::string name);

	bool gettostack(std::string name);

	template <typename T>
	T get(std::string name);

	void push(bool value);
	void push(int value);
	void push(float value);
	void push(std::string value);

	std::vector<std::string> getKeys();

	void parse(std::string name, ...);

	template<typename T>
	T LuaFile::parseVal(std::string name, ...);

	std::string nspace;

private:
	lua_State* L;
	int lev;
};

template <typename T>
T LuaFile::get(std::string name){
	if(!L) {
		printError(name, "Script was not loaded");
		return void;
	}

	T result;
	if(gettostack(name)) {
		result = get<T>(nspace + "." + name);
	} else {
		result = void;
	}

	clean();
	return result;
}

template<typename T>
std::vector<T> LuaFile::getVector(std::string name){
	std::vector<T> v;

	if(!lua_gettostack(name.c_str())) {
        printError(name, "Array not found");
        clean();
        return std::vector<T>();
    }

	if (!lua_isnil(L, -1)){
		lua_pushnil(L);
		while (lua_next(L, -2)) {
        	v.push_back((T)lua_tonumber(L, -1));
        	lua_pop(L, 1);
    	}
	}
	
	clean();
	return v;
}

template<typename T>
T LuaFile::parseVal(std::string name, ...){

}

// Template Specifications
/*template <>
inline bool LuaFile::get(std::string name);
template <>
inline float LuaFile::get(std::string name);
template <>
inline int LuaFile::get(std::string name);
template <>
inline std::string LuaFile::get(std::string name);
*/

//Default Templates
template <typename T>
T LuaFile::get(std::string name){
	return void;
}

// Template Specifications
template <>
inline bool LuaFile::get(std::string name){
	return (bool)lua_toboolean(L,-1);
}
template <>
inline int LuaFile::get(std::string name){
	if(!lua_isnumber(L,-1)){
		printError(name, "not a integer");
	}
	return (int)lua_tonumber(L,-1);
}
template <>
inline float LuaFile::get(std::string name){
	if(!lua_isnumber(L,-1)){
		printError(name, "not a float");
	}
	return (float)lua_tonumber(L,-1);
}
template <>
inline std::string LuaFile::get(std::string name){
	if(lua_isstring(L,-1)){
		return std::string(lua_tostring(L, -1));
	} else {
		printError(name, "not a string");
		return "ERROR";
	}
}

void LuaFile::push(bool value){
	lua_pushboolean(L, value);
}
void LuaFile::push(int value){
	lua_pushinteger(L, value);
}
void LuaFile::push(float value){
	lua_pushnumber(L, value);
}
void LuaFile::push(std::string value){
	lua_pushstring(L, value.c_str());
}
#endif