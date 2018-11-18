#ifndef LUARIUM_LEVEL_H
#define LUARIUM_LEVEL_H

#include <string>
#include <vector>

#include "luarium/core/object.h"

namespace Map{
	namespace luabridge = LB;
public:
	bool load(std::string path){
		LB::luaL_dofile(L, path + "level.lua");
		LB::luaL_openlibs(L);
  		lua_pcall(L, 0, 0, 0);
	}

	void unload();

	bool save();

private:
	lua_State* L = luaL_newstate();
}


class Level : public Object {

	Level(std::string path);
	~Level();

	bool load(std::string path);
}

#endif