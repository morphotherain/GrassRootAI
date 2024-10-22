// SingletonBase.h
#pragma once

template<typename T>
struct SingletonBase {
public:
    SingletonBase() {}
    ~SingletonBase() {}

    static T* getInstance() {
        static T instance;
        return &instance;
    }
};
