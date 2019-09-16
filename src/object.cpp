#include "luarium/object.h"

Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	pos = pos; rot = rot; scl = scl;
}

Object::~Object(){
	parent = nullptr;
}

// Calculates the object's spatial properties relative to global space
Object::spatial Object::derefrence() {
	spatial o;
	
	if(parent != nullptr) {
		spatial d_parent = parent->derefrence();

		o.pos = pos + d_parent.pos;
		o.rot = rot + d_parent.rot;
		o.scl = scl * d_parent.scl;
	}

	return o;
}

// Moves an object from one parent into another (Destination << Input)
void Object::operator<<(Object::ptr &o) {
	o->parent = this;
	components.push_back(std::move(o));
}

void Object::operator<<(std::vector<Object::ptr> &o_vec){
	for(auto it = o_vec.begin(); it != o_vec.end(); it++){
		(*it)->parent = this;
		components.push_back(std::move(*it));
		o_vec.erase(it);
	}
}

// Deletes an object from the parent's vector 
void Object::operator-=(Object::ptr &o) {
	std::vector<Object::ptr>* vec = &(this->components);
	// auto it = std::find_if(vec->begin(), vec->end(), [&](std::unique_ptr<Object> &p){
	// 	return p.get() == o.get();
	// });
	// if(it != vec->end())
	// 	vec->erase(it);

	std::remove(vec->begin(), vec->end(), o);
	o->parent = nullptr;
}

// Returns a refrence to the specified element (a unique pointer), casted to type T.
// This allows whatever method is to use the values of the original object type, not just Object's values
template<class T> std::unique_ptr<T>& Object::operator[](size_t index) {
	return dynamic_cast<T&>(*components.at(index)); 
}

// Same as above, but first searches for the component by id
template<class T> std::unique_ptr<T>& Object::operator[](std::string id) { 
	size_t index = std::find_if(components.begin(), components.end(), [id](const Object::ptr& o) -> bool { return o->id == id; });
	return dynamic_cast<T&>(*components.at(index));
}

void Object::jload(Json::Value j) {
	this->id = j.get("id", "").asString();
	this->type = j.get("type", "BlankObject").asString();

	Json::Value jPos = j["pos"];
	Json::Value jRot = j["rot"];
	Json::Value jScl = j["scl"];

	this->pos = glm::vec3(jPos[0].asFloat(), jPos[1].asFloat(), jPos[2].asFloat());
	this->rot = glm::vec3(jRot[0].asFloat(), jRot[1].asFloat(), jRot[2].asFloat());
	this->scl = glm::vec3(jScl[0].asFloat(), jScl[1].asFloat(), jScl[2].asFloat());

	Json::Value items = j["components"];

    for(unsigned i = 0; i < items.size(); i++) {
		Object::ptr o = ObjFactory::createObject(items[i]["type"].asString());
		o->jload(items[i]);
		o->parent = this;
		components.push_back(std::move(o));
	}
}


Object::ptr ObjFactory::createObject(std::string const& s) {
	ObjFactory::map_type::iterator it = getMap()->find(s);
	if(it == getMap()->end()) {
		LuariumLog("Could not find object type \"" + s + "\", type not registered", 2);
		return nullptr;
	}
	return it->second();
}

bool const ObjFactory::registerType(const char* name, Object::ptr (*create_f)()) {
	getMap()->emplace(name, create_f);	
	return true;
}

std::shared_ptr<ObjFactory::map_type> ObjFactory::getMap() {
	if(!typemap) { typemap = std::make_shared<ObjFactory::map_type>(); } 
	return typemap;
}

void BlankObject::jload(Json::Value j) {
	Object::jload(j);
}
