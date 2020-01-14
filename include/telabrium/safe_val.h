#pragma once

#include <functional>
#include "telabrium/utility.h"

// A bunch of operators for types that need getters and setters
template<typename T>
struct safe_val {
    safe_val(std::function<T()> getter, std::function<T(T)> setter);
    safe_val();

    void setFuncs(std::function<T()> getter, std::function<T(T)> setter);

    T operator=(const T& val);
    T operator()(const T& val);
    T operator()();

    T operator+=(const T& val);
    T operator-=(const T& val);
    T operator*=(const T& val);
    T operator/=(const T& val);

    T operator+(const T& val);
    T operator-(const T& val);
    T operator*(const T& val);
    T operator/(const T& val);

protected:
    std::function<T()> get;
    std::function<T(T)> set;
};


/// Templates must be stored in headers

template<typename T>
safe_val<T>::safe_val(std::function<T()> getter, std::function<T(T)> setter) {
    get = getter;
    set = setter;
}

template<typename T>
safe_val<T>::safe_val() {
    
}

template<typename T>
void safe_val<T>::setFuncs(std::function<T()> getter, std::function<T(T)> setter) {
    get = getter; 
    set = setter;
}


template<typename T>
T safe_val<T>::operator=(const T& val) {
    if(!set) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return set(val);
}

template<typename T>
T safe_val<T>::operator()(const T& val) {
    if(!set) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return set(val);
}

template<typename T>
T safe_val<T>::operator()() {
    if(!get) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return get();
}


template<typename T>
T safe_val<T>::operator+=(const T& val) {
    if(!set) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return set(get() + val);
}

template<typename T>
T safe_val<T>::operator-=(const T& val) {
    if(!set) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return set(get() - val);
}

template<typename T>
T safe_val<T>::operator*=(const T& val) {
    if(!set) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return set(get() * val);
}

template<typename T>
T safe_val<T>::operator/=(const T& val) {
    if(!set) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return set(get() / val);
}


template<typename T>
T safe_val<T>::operator+(const T& val) {
    if(!get) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return get() + val;
}

template<typename T>
T safe_val<T>::operator-(const T& val) {
    if(!get) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return get() - val;
}

template<typename T>
T safe_val<T>::operator*(const T& val) {
    if(!get) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return get() * val;
}

template<typename T>
T safe_val<T>::operator/(const T& val) {
    if(!get) {
        TelabriumLog("Can't access value, has this safe_val been assigned functions?", 3);
    }
    return get() / val;
}