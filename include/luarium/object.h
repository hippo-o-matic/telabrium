#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <memory>

#include "glm/glm.hpp"
#include "json/json.h"
#include "json/json-forwards.h"

#include "luarium/utility.h"

class Object {
public:
    typedef std::unique_ptr<Object> ptr;

    // Kind of jank, but this is for any functions that want to return object values without returning their component vectors or parents
    struct spatial {
        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 scl;
    };

	Object(
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 scl = glm::vec3(1.0f, 1.0f, 1.0f)
    );

	virtual ~Object();

	std::string id;
    std::string type;

    // Spatial properties relative to parent
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scl;

	void operator<<(Object::ptr &o); // Move an object from one parent to another
    void operator<<(std::vector<Object::ptr> &o_vec); // Move a vector of objects from one parent to another
	void operator-=(Object::ptr &o); // Remove an object from an object's components

    template<class T> std::unique_ptr<T>& operator[](size_t index); // Operator that returns a component in this object's vector by index
    template<class T> std::unique_ptr<T>&  operator[](std::string id); // Operator that returns the component in this object's vector that matches the id

    spatial derefrence(); // Calculates the object's spatial properties relative to global space

    template<class T>
    std::unique_ptr<T>& to() { return dynamic_cast<std::unique_ptr<T>&>(*this); } // Method for casting a type back to it's native type, [TODO] make automatic when any Object::ptr is refrenced

    // A vector that holds objects with no parent, in practice shouldn't hold anything other than Level objects
    // static std::vector<Object::ptr> global;

protected:
	Object* parent; // Raw pointer to the objects parent
	std::vector<Object::ptr> components; // Vector of the objects sub-components

    virtual void jload(Json::Value j);
};

// A class that registers, creates, and defines new Objects
class ObjFactory {
    typedef Object::ptr (*createFunc)(); // A function pointer for the creation functions
    typedef std::map<std::string, createFunc> map_type;
    // typedef void (Object::*mfptr)(Object::ptr*, Json::Value); // Member function pointer for the value function

public:
    static Object::ptr createObject(std::string const& s); // Creates a registered object and returns its unique pointer

    static bool const registerType(const char* name, Object::ptr (*create_f)());
	
private:
    static std::shared_ptr<map_type> getMap();
    inline static std::shared_ptr<map_type> typemap = nullptr; // The map of all the types and their functions
};

/* Creates a register function for the object and calls it, place inside class definition
* luarium_obj_create(): A function that returns a new NAME
* VALUE_F(Object::ptr*, Json::Value): A function address of a function that fills out an object based off of a Json::Value,
* should take a pointer to a unique pointer of an object (Object::ptr*) and a Json::Value
* luarium_obj_reg: a trick using static initialization order to register the type before main()*/
#define LUARIUM_REGISTER_OBJECT(NAME) \
    inline static Object::ptr luarium_obj_create() { return std::make_unique<NAME>(); }; \
    inline static bool luarium_obj_reg = ObjFactory::registerType (#NAME, NAME::luarium_obj_create)


class BlankObject : public Object {
public:
    BlankObject() = default;

    void jload(Json::Value j);

private:
    LUARIUM_REGISTER_OBJECT(BlankObject);
};

#endif
