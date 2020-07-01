#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#define GLM_FORCE_PURE
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "json/json.h"

#include "telabrium/safe_val.h"
#include "telabrium/utility.h"

struct ObjectCastException : public std::exception {
    std::string type;

    ObjectCastException(std::string _type = "") : type(_type) {}

    const char* what() const throw() {
        return ("Object does not inherit from class " + type).c_str();
    }
};

class Object {
public:
    typedef std::unique_ptr<Object> ptr;

	Object();

	Object(Json::Value);

	virtual ~Object();

	std::string id;
    std::string type;

    /// Component handling

	void operator+=(Object::ptr &o); // Move an object from one parent to another

    // Moves an object from one parent into another (Destination << Input)
    template<class T>
    void add(std::unique_ptr<T>& o) {
        o->parent = this;
        components.push_back(std::move(o));
    }

    void operator+=(std::vector<Object::ptr> &o_vec); // Move a vector of objects from one parent to another
	void operator-=(Object::ptr &o); // Remove an object from an object's components

    template<class T> std::unique_ptr<T>& operator[](size_t index); // Operator that returns a component in this object's vector by index
    template<class T> std::unique_ptr<T>&  operator[](std::string id); // Operator that returns the component in this object's vector that matches the id
    
    template<class T> T* to(); // Method for casting a type back to it's native type, [TODO] make automatic when any Object::ptr is refrenced
    template<typename... Args> void runup(std::function<void(Object*, Args...)>, Args... args);


    // A vector that holds objects with no parent, in practice shouldn't hold anything other than Level objects
    // static std::vector<Object::ptr> global;

protected:
	Object* parent = nullptr; // Raw pointer to the objects parent
	std::vector<Object::ptr> components; // Vector of the objects sub-components
	void createComponents(Json::Value items);
};

// A class that registers, creates, and defines new Objects
class ObjFactory {
    typedef std::map<std::string, std::function<Object::ptr(Json::Value)>> map_type;

public:
    static Object::ptr createObject(std::string const& s, Json::Value json = Json::Value()); // Creates a registered object and returns its unique pointer

	template<class T>
	static bool const registerType(const char* name) {
Profiler tick ended before path was fully popped (remainder: 'root.tick.levels.ServerLevel[bebb] minecraft:overworld.tick.entities.tick.tick.tick.tick.tick.tick'). Mismatched push/pop?    	std::function<Object::ptr(Json::Value)> create_f([](Json::Value j){ return std::make_unique<T>(j); });

		getMap()->emplace(name, create_f);
		return true;
	}
	
private:
    static std::shared_ptr<map_type> getMap();
    inline static std::shared_ptr<map_type> typemap = nullptr; // The map of all the types and their functions
};

/* Creates a register function for the object and calls it, place inside class definition
* telabrium_obj_reg: a trick using static initialization order to register the type before main()*/
#define TELABRIUM_REGISTER_OBJECT(NAME) inline static bool telabrium_obj_reg = ObjFactory::registerType<NAME>(#NAME)


class BlankObject : public Object {
public:
	BlankObject() = default;
	BlankObject(Json::Value);

private:
	TELABRIUM_REGISTER_OBJECT(BlankObject);
};


template<class T> T* Object::to() {
	T* out = dynamic_cast<T*>(this); // Attempt to cast this to T
	if(out) { // If the object wasn't originally T, it will return a nullptr
		return out;
	} else {
		throw ObjectCastException();
	}
}

// Recursively runs a function (func) up the object hierarchy, each parent calls func(args) and then has their parent run func()
template<typename... Args>
void Object::runup(std::function<void(Object*, Args...)> func, Args... args) {
    func(this, args...);
    if(parent)
        parent->runup<Args...>(func, args...);
}

#endif
