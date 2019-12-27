#pragma once

#include <vector>
#include <functional>

#include "telabrium/object.h"

// An automation class that takes a vector of objects (T) and runs an object (obj_func) for each one with args of type (Args...).
// Also runs (task_func) once everytime the vector is run
template <class T, typename... Args>
class Task {
public:
    Task(std::function<void(Args... args)> obj_f); // Constructor for lambda functions
    Task(std::function<void(Args... args)> obj_f, std::function<void(Args... args)> task_f);
    Task(void (T::*obj_f)(Args... args)); // Constructor for member function pointers
    Task(void (T::*obj_f)(Args...), void (*task_f)(Args...));

    void addObj(T*);
    void addObj(std::vector<T*>);

    void removeObj(T*);
    void removeObj(std::vector<T*>);

    void setObjFunc(std::function<void(Args... args)>); // Assign the obj_func
    void setObjFunc(void (T::*f)(Args...));

    void setTaskFunc(std::function<void(Args... args)>); // Assign the task_func
    void setTaskFunc(void (*f)(Args...));

    void exec(Args...);

    size_t getObjCount();

private:
    std::function<void(T*, Args... args)> obj_func; // Function to be run on every object in task_objects
    std::function<void(Args... args)> task_func; // Function to be run only once when the task is executed
    std::vector<T*> task_objects; // Vector of objects that will run the obj function as a member function
};

// Constructors for assigning functions

template <class T, typename... Args>
Task<T, Args...>::Task(std::function<void(Args...)> obj_f) {
    setObjFunc(obj_f);
}

template <class T, typename... Args>
Task<T, Args...>::Task(std::function<void(Args... args)> obj_f, std::function<void(Args... args)> task_f) {
    setObjFunc(obj_f);
    setTaskFunc(task_f);
}

template <class T, typename... Args>
Task<T, Args...>::Task(void (T::*obj_f)(Args...)) {
    setObjFunc(obj_f);
}

template <class T, typename... Args>
Task<T, Args...>::Task(void (T::*obj_f)(Args...), void (*task_f)(Args...)) {
    setObjFunc(obj_f);
    setTaskFunc(task_f);
}

// Add and remove objects

template <class T, typename... Args>
void Task<T, Args...>::addObj(T* obj) {
    task_objects.push_back(obj);
}

template <class T, typename... Args>
void Task<T, Args...>::addObj(std::vector<T*> objs) {
    for (auto it = objs.begin(); it != objs.end(); it++) {
        task_objects.push_back(objs[it]);
    }
}

template <class T, typename... Args>
void Task<T, Args...>::removeObj(T* obj) {
    task_objects.erase(std::find(task_objects.begin(), task_objects.end(), obj));
}

template <class T, typename... Args>
void Task<T, Args...>::removeObj(std::vector<T*> objs) {
    for (auto it = objs.begin(); it != objs.end(); it++) {
        task_objects.erase(std::find(task_objects.begin(), task_objects.end(), objs));
    }
}

// Assign object and task functions

template <class T, typename... Args>
void Task<T, Args...>::setObjFunc(std::function<void(Args... args)> f) {
    obj_func = [f](T* obj, Args... args) {
        obj->f(args...);
    };
}

template <class T, typename... Args>
void Task<T, Args...>::setObjFunc(void (T::*f)(Args...)) {
    obj_func = [f](T* obj, Args... args) {
        (obj->*f)(args...);
    };
}


template <class T, typename... Args>
void Task<T, Args...>::setTaskFunc(std::function<void(Args... args)> f) {
    task_func = f;
}

template <class T, typename... Args>
void Task<T, Args...>::setTaskFunc(void (*f)(Args...)) {
    task_func = [f](Args... args) {
        f(args...);
    };
}

// Execute the stored functions
template <class T, typename... Args>
void Task<T, Args...>::exec(Args... args) {
    if (task_func != nullptr) task_func(args...); // Execute the per-execution function
    for (auto it = task_objects.begin(); it != task_objects.end(); it++) {
        if (*it != nullptr && obj_func != nullptr){
            obj_func(*it, args...); // Execute the per-object function
        } 
    }
}


template <class T, typename... Args>
size_t Task<T, Args...>::getObjCount() {
    return task_objects.size();
}