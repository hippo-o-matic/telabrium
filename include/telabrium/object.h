#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

#include "json/json.h"
#include "json/json-forwards.h"

#include "telabrium/utility.h"

struct spatial {
    spatial(
        Object* o,
        glm::vec3 (Object::*getter_func)(),
        glm::vec3 (Object::*setter_func)(glm::vec3)
    );

    Object* obj;
    // Getter and setter function pointers, given by the constructor
    glm::vec3 (Object::*get)();
    glm::vec3 (Object::*set)(glm::vec3);

    glm::vec3 relative(spatial);
    glm::vec3 relative(spatial, glm::vec3);

    float x();
    float y();
    float z();

    float x(float);
    float y(float);
    float z(float);
};

class Object {
public:
    typedef std::unique_ptr<Object> ptr;

	Object(
        glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 rot = glm::vec3(0.0f),
        glm::vec3 scl = glm::vec3(1.0f)
    );

	virtual ~Object();

	std::string id;
    std::string type;

    glm::vec3 pos(); // Returns the objects position
    glm::vec3 pos(glm::vec3); // Sets the objects position

    // The reason both the getter/setters are the same name as this spatial struct is for ease of use. This way thing.pos() and thing.pos.x() both are valid
    // spatial pos;

    glm::vec3 getPos();
    glm::vec3 getRelativePos();
    void setPos(glm::vec3);
    void setRelativePos(glm::vec3);

    glm::vec3 getRot();
    glm::vec3 getRotRad();
    glm::vec3 getRelativeRot();
    void setRot(glm::vec3);
    void setRelativeRot(glm::vec3);
    void setRotX(float);
    void setRotY(float);
    void setRotZ(float);

    glm::vec3 getScl();
    glm::vec3 getRelativeScale();
    void setScl(glm::vec3);
    void setRelativeScale(glm::vec3);
    void setSclX(float);
    void setSclY(float);
    void setSclZ(float);

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
    
    template<class T>
    std::unique_ptr<T>& to() { return dynamic_cast<std::unique_ptr<T>&>(*this); } // Method for casting a type back to it's native type, [TODO] make automatic when any Object::ptr is refrenced

    // A vector that holds objects with no parent, in practice shouldn't hold anything other than Level objects
    // static std::vector<Object::ptr> global;

protected:
	Object* parent; // Raw pointer to the objects parent
	std::vector<Object::ptr> components; // Vector of the objects sub-components

    virtual void jload(Json::Value j);

private:
    // Spatial properties relative to parent
    glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
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
* telabrium_obj_create(): A function that returns a new NAME
* VALUE_F(Object::ptr*, Json::Value): A function address of a function that fills out an object based off of a Json::Value,
* should take a pointer to a unique pointer of an object (Object::ptr*) and a Json::Value
* telabrium_obj_reg: a trick using static initialization order to register the type before main()*/
#define LUARIUM_REGISTER_OBJECT(NAME) \
    inline static Object::ptr telabrium_obj_create() { return std::make_unique<NAME>(); }; \
    inline static bool telabrium_obj_reg = ObjFactory::registerType (#NAME, NAME::telabrium_obj_create)


class BlankObject : public Object {
public:
    BlankObject() = default;

    void jload(Json::Value j);

private:
    LUARIUM_REGISTER_OBJECT(BlankObject);
};

#endif
