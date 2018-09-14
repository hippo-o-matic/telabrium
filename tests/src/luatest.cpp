#include <iostream>
#include "LuaBridge/LuaBridge.h"

using luabridge = lua;
int main() {
	lua::lua_State* L = lua::luaL_newstate();
	lua::luaL_dofile(L, "script.lua");
	lua::luaL_openlibs(L);
	lua::lua_pcall(L, 0, 0, 0);
	LuaRef s = lua::getGlobal(L, "testString");
	LuaRef n = lua::getGlobal(L, "number");
	std::string luaString = s.cast<std::string>();
	int answer = n.cast<int>();
	std::cout << luaString << std::endl;
	std::cout << "And here's our number:" << answer << std::endl;
}