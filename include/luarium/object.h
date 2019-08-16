#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <memory>
#include <functional>

#include "glm/glm.hpp"
#include "json/json.h"
#include "json/json-forwards.h"

#include "luarium/utility.h"

class Object {
public:
    typedef std::unique_ptr<Object> ptr;

    // Kind of jank, but this is for any functions that want to return object values without returning their component vectors or parents
    struct spatial {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale;
    };

	Object(
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 scl = glm::vec3(1.0f, 1.0f, 1.0f)
    );

	~Object();

	std::string id;

    // Spatial properties relative to parent
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

	void operator<<(Object::ptr &o); // Move an object from one parent to another
    void operator<<(std::vector<Object::ptr> &o_vec); // Move a vector of objects from one parent to another
	void operator-=(Object::ptr &o); // Remove an object from an object's components

    template<class T> std::unique_ptr<T>& operator[](size_t index); // Operator that returns a component in this object's vector by index
    template<class T> std::unique_ptr<T>&  operator[](std::string id); // Operator that returns the component in this object's vector that matches the id

    spatial derefrence(); // Calculates the object's spatial properties relative to global space

    // A vector that holds objects with no parent, in practice shouldn't hold anything other than Level objects
    static std::vector<Object::ptr> global;

protected:
	Object* parent; // Raw pointer to the objects parent
	std::vector<Object::ptr> components; // Vector of the objects sub-components

    void jload(Json::Value j);

    // Object register members
    static std::function<Object::ptr()> luarium_obj_create;
    std::function<void(Json::Value)> value_f;
    static bool luarium_obj_reg;
};

// A class that registers, creates, and defines new Objects
class ObjFactory {
    struct object_functions{
        std::function<Object::ptr()> create_f; // A pointer to a function that allows us to create a new instance of the object type
        std::function<void(Json::Value)> value_f; // A member function pointer to a function that will allow us to define objects with a Json::Value
    };
    typedef std::map<std::string, object_functions> map_type;
    typedef void (Object::*mfptr)(Json::Value); // Member function pointer for the value function

public:
    static Object::ptr createInstance(std::string const& s); // Creates a registered object and returns its unique pointer

    static bool const registerType(const char* name, std::function<Object::ptr()> create_f, std::function<void> value_f(Json::Value));

    template<class T>
	void (T::*getValueFunc(std::string const& s))(Json::Value) { // Returns the value function provided when registering type
        ObjFactory::map_type::iterator it = getMap()->find(s);
        if(it == getMap()->end()){
            Luarium::log("Could not find object type \"" + s + "\", type not registered", 1);
            return nullptr;
        }
        return it->second.value_func;
    }
	
private:
    static std::shared_ptr<map_type> getMap();
    static std::shared_ptr<map_type> typemap; // The map of all the types and their functions
};


/* Creates a register function for the object and calls it, place inside class definition
* luarium_obj_create(): A function that returns a new NAME
* VALUE_F(): A function address of a function that fills out an object based off of a Json::Value
* luarium_obj_reg: a trick using static initialization order to register the type before main()*/    
#define LUARIUM_REGISTER_OBJECT(NAME, VALUE_F) \
    static std::function<Object::ptr()> NAME::luarium_obj_create = [](){ return std::make_unique<NAME>(); }; \
    std::function<void(Json::Value)> NAME::value_f = VALUE_F; \
    static bool NAME::luarium_obj_reg = ObjFactory::registerType (#NAME, NAME::luarium_obj_create, VALUE_F)

#endif
