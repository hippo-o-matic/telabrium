#include "telabrium/object.h"

Object::Object(glm::vec3 positon, glm::vec3 rotation, glm::vec3 scale) {
	pos.setFuncs(
		std::function<glm::vec3()>([this](){ return this->getPos(); }), 
		std::function<glm::vec3(glm::vec3)>([this](glm::vec3 val){ return this->setPos(val); })
	);
	
	rot.setFuncs(
		std::function<glm::vec3()>([this](){ return this->getRot(); }), 
		std::function<glm::vec3(glm::vec3)>([this](glm::vec3 val){ return this->setRot(val); })
	);

	scl.setFuncs(
		std::function<glm::vec3()>([this](){ return this->getScl(); }), 
		std::function<glm::vec3(glm::vec3)>([this](glm::vec3 val){ return this->setScl(val); })
	);

    pos = position;
    rot = rotation;
    scl = scale;
}

Object::~Object() {
	parent = nullptr;
}

// Constructor for the spatial functors
spatial::spatial(
	std::function<glm::vec3()> getter_f,
	std::function<glm::vec3(glm::vec3)> setter_f
) {
	safe_val(getter_f, setter_f);
	
    // Sets the functions the .x, .y and .z components will use when getting or setting values
	x = safe_val<double>(
		std::function<double()>( [this](){ return this->get().x; } ), 
		std::function<double(double)>( [this](double val){ return this->set(glm::vec3(val, this->get().y, this->get().z)).x; } )
	);

	y = safe_val<double>(
		std::function<double()>( [this](){ return this->get().y; } ), 
		std::function<double(double)>( [this](double val){ return this->set(glm::vec3(this->get().x, val, this->get().z)).x; } )
	);

	z = safe_val<double>(
		std::function<double()>( [this](){ return this->get().z; } ), 
		std::function<double(double)>( [this](double val){ return this->set(glm::vec3(this->get().x, this->get().y, val)).x; } )
	);
}

// Override the safe_val operator
glm::vec3 spatial::operator=(glm::vec3 val) {
	return set(val);
}

// glm::vec3 spatial::relative(spatial ref) {
// 	return (obj->*get)() - (ref.obj->*get)();
// }
// glm::vec3 spatial::relative(spatial ref, glm::vec3 dest) {
// 	return (obj->*set)((ref.obj->*get)() + dest);
// }

glm::vec3 Object::getPos() {
	return position;
}

// Sets the objects position and updates all of its components positions 
glm::vec3 Object::setPos(glm::vec3 dest){
	for(auto& i : components) {
		i->pos += position - dest; // Component position + change in my position
	}
	
	return position = dest;
}


// Returns a degree measurement of the object in global space
glm::vec3 Object::getRot(){
	return rotation;
}

// Sets the objects rotation in degrees and updates all its components rotations
glm::vec3 Object::setRot(glm::vec3 in) {
	rotation = glm::mod(in, glm::vec3(360)); // Keep measurement between 0 and 360 degrees

	for(auto& i : components) {
		i->setRot(i->getRot() + rotation); // Set the components new rotation

		// Rotate the components position around this object
		glm::vec3 rad = glm::radians(rot());

		glm::vec3 newPos = i->pos() - i->parent->pos();
		newPos = glm::rotate(newPos, rad.x, glm::vec3(1,0,0));
		newPos = glm::rotate(newPos, rad.y, glm::vec3(0,1,0));
		newPos = glm::rotate(newPos, rad.z, glm::vec3(0,0,1));
		i->pos((i->parent->pos() + newPos));
	}
	
	return rotation;
}

glm::vec3 Object::getScl() {
	return scale;
}

// Sets the objects scale and updates its components scales
glm::vec3 Object::setScl(glm::vec3 in) {
	scale = in;
	for(auto& i : components) {
		i->scl *= this->scl();
	}

	return scale;
}


// Moves an object from one parent into another (Destination << Input)
void Object::operator+=(Object::ptr &o) {
	// o->parent-= o;

	o->parent = this;
	components.push_back(std::move(o));
}

// Moves several objects from one parent to another
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
