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

// Creates a registery function for the object, place inside class definition
#define DECLARE_OBJECT_TYPE(NAME) \
    static ObjectRegister<NAME> reg

// Registers the object as one that can be created, place after class definition
// (The name of the function, a json parsing function)
#define DEFINE_OBJECT_TYPE(NAME, FUNC) \
    ObjectRegister<NAME> NAME::reg(#NAME, FUNC)

class Object {
    typedef std::unique_ptr<Object> ptr;
    friend struct ObjectFactory; // A struct that allows us to generate new Object instances

    // Kind of jank, but this is for any functions that want to return object values without returning their component vectors or parents
    struct spatial {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale;
    };

public:
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
};



struct ObjectFactory {
    struct object_functions{
        Object::ptr (*create_f)(); // A pointer to a function that allows us to create a new instance of the object type
        void (Object::*value_func)(Json::Value); // This one's a bit weird, but its a pointer to a member function, so we can fill out an object's values later
    };
    typedef std::map<std::string, object_functions> map_type;
    typedef void (Object::*mfptr)(Json::Value); // Member function pointer for the value function

public:
    static Object::ptr createInstance(std::string const& s) {
        map_type::iterator it = getMap()->find(s);
        if(it == getMap()->end()) {
            Luarium::log("Could not find object type \"" + s + "\", type not registered", 1);
            return 0;
        }
        return it->second.create_f();
    }

    template<class T> static void (T::*getValueFunc(std::string const& s))(Json::Value) { // Returns the value function provided when registering type
        map_type::iterator it = getMap()->find(s);
        if(it == getMap()->end()){
            Luarium::log("Could not find object type \"" + s + "\", type not registered", 1);
            return 0;
        }
        return it->second.value_func;
    }

protected:
    static std::shared_ptr<map_type> getMap() {
        if(!typemap) { typemap = std::shared_ptr<map_type>(); } 
        return typemap; 
    }

    // Object factories for different object creation
    template<typename T> static Object::ptr create() { return std::make_unique<T>(); } // A function that creates and returns a new instance of any Object

private:
    static std::shared_ptr<map_type> typemap;
};


template<typename T>
struct ObjectRegister : ObjectFactory {
    ObjectRegister(const char* s, void (T::*value_func)(Json::Value)) {
        object_functions f = {
            &create<T>,
            static_cast<mfptr>(value_func),
        };

        getMap()->insert(std::pair<std::string, object_functions>(s, f));
    }
};

#endif
