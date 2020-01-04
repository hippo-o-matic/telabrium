#include "telabrium/object.h"

Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	pos = spatial()

	setPos(pos);
	setRot(rot);
	setScl(scl);
}

Object::~Object(){
	parent = nullptr;
}

// Constructor for the spatial functors
spatial::spatial(
	Object* o,
	glm::vec3 (Object::*getter_func)(),
	glm::vec3 (Object::*setter_func)(glm::vec3)
) {
	obj = o;
	get = getter_func;
	set = setter_func;
}

glm::vec3 spatial::relative(spatial ref) {
	return (obj->*get)() - ref.(obj->*get)();
}
glm::vec3 spatial::relative(spatial ref, glm::vec3 dest) {
	return (obj->*set)(ref.get() + dest);
}

// Pointer to member function syntax is icky, but essentially what this says is that
// whatever object is calling this function (foo.x()) is calling its respective get 
// and set commands, specified in the constructor of this spatial.

// Returns the x value of this spatial property
float spatial::x() {
	return (obj->*get)().x;
}
// Returns the y value of this spatial property
float spatial::y() {
	return (obj->*get)().y;
}
// Returns the z value of this spatial property
float spatial::z() {
	return (obj->*get)().z;
}

// Sets and returns the x value of this spatial property
float spatial::x(float x) {
	(obj->*set)(glm::vec3(x, (obj->*get)().y, (obj->*get)().z));
	return (obj->*get)().x;
}
// Sets and returns the y value of this spatial property
float spatial::y(float y) {
	(obj->*set)(glm::vec3((obj->*get)().x, y, (obj->*get)().z));
	return (obj->*get)().y;
}
// Sets and returns the z value of this spatial property
float spatial::z(float z) {
	(obj->*set)(glm::vec3((obj->*get)().x, (obj->*get)().y, z));
	return (obj->*get)().z;
}

// Returns object's position in global space
glm::vec3 Object::pos(){
	return position;
}

glm::vec3 Object::pos(glm::vec3 dest){
	for(auto& i : components) {
		i->pos(pos() + position - dest); // Component position + change in my position
	}
	position = dest;
}

// Returns object's position relative to its parent
glm::vec3 Object::getRelativePos() {
	return position - parent->getPos();
}

// Sets the objects position and updates all of its components positions 
void Object::setPos(glm::vec3 in) {
	for(auto& i : components) {
		i->setPos(getPos() + position - in); // Component position + change in my position
	}
	position = in;
}

void Object::setPosX(float x) {
	setPos(glm::vec3(x, getPos().y, getPos().z));
}

// Sets the objects position relative to its parent
void Object::setRelativePos(glm::vec3 in) {
	setPos(parent->getPos() + in);
}

// Returns a degree measurement of the object in global space
glm::vec3 Object::getRot(){
	return rotation;
}

// Returns a radian measurement of the object in global space
glm::vec3 Object::getRotRad() {
	return glm::radians(getRot());
}

void Object::setRot(glm::vec3 in) {
	rotation = glm::mod(in, glm::vec3(360));

	for(auto& i : components) {
		i->setRot(i->getRot() + rotation); // Set the components new rotation

		// Rotate the components position around this object
		glm::vec3 newPos = i->getRelativePos();
		newPos = glm::rotate(newPos, getRotRad().x, glm::vec3(1,0,0));
		newPos = glm::rotate(newPos, getRotRad().y, glm::vec3(0,1,0));
		newPos = glm::rotate(newPos, getRotRad().z, glm::vec3(0,0,1));
		i->setRelativePos(newPos);
	}
}

void Object::setRelativeRot(glm::vec3 in) {
	setRot(in + parent->getRot());
}

glm::vec3 Object::getScl() {
	return scale;
}

void Object::setScl(glm::vec3 in) {
	scale = in;
	for(auto& i : components) {
		i->setScl(getScl() * scale);
	}
}


// Moves an object from one parent into another (Destination << Input)
void Object::operator+=(Object::ptr &o) {
	// o->parent-= o;

	o->parent = this;
	components.push_back(std::move(o));
}

void Object::operator+=(std::vector<Object::ptr> &o_vec){
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

	setPos(glm::vec3(jPos[0].asFloat(), jPos[1].asFloat(), jPos[2].asFloat()));
	setRot(glm::vec3(jRot[0].asFloat(), jRot[1].asFloat(), jRot[2].asFloat()));
	setScl(glm::vec3(jScl[0].asFloat(), jScl[1].asFloat(), jScl[2].asFloat()));

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
		TelabriumLog("Could not find object type \"" + s + "\", type not registered", 2);
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
