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
    typedef std::unique_ptr<Object> ptr;
    friend struct ObjFactory; // A struct that allows us to generate new Object instances

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

	void DECLARE_OBJECT_TYPE(Object);
protected:
	Object* parent; // Raw pointer to the objects parent
	std::vector<Object::ptr> components; // Vector of the objects sub-components

    void jload(Json::Value j);
};


struct ObjFactory {
    struct object_functions{
        Object::ptr (*create_f)(); // A pointer to a function that allows us to create a new instance of the object type
        void (Object::*value_func)(Json::Value); // This one's a bit weird, but its a pointer to a member function, so we can fill out an object's values later
    };
    typedef std::map<std::string, object_functions> map_type;
    typedef void (Object::*mfptr)(Json::Value); // Member function pointer for the value function

public:
    static Object::ptr createInstance(std::string const& s); // Creates a registered object and returns its unique pointer

    template<class T>
	static void (T::*getValueFunc(std::string const& s))(Json::Value);

protected:
    static std::shared_ptr<map_type> getMap();

    template<class T>
	static Object::ptr create(); // A function that creates and returns a new instance of any Object
	
private:
    static std::shared_ptr<map_type> typemap;
};


template<class T>
struct ObjRegister : ObjFactory {
    ObjRegister(const char* s, void (T::*value_func)(Json::Value));
};


// Creates a registery function for the object, place inside class definition
#define DECLARE_OBJECT_TYPE(NAME) \
	static ObjRegister<NAME> reg

// Registers the object as one that can be created, place after class definition
// (The name of the function, a json parsing function)
#define DEFINE_OBJECT_TYPE(NAME, FUNC) \
    ObjRegister<NAME> NAME::reg(#NAME, FUNC)

#endif
