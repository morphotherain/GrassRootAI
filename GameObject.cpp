#include "GameObject.h"

// 初始化静态的对象映射表指针为nullptr
std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> GameObjectMgr::p_mapObject = nullptr;

//// 初始化静态单例对象（C++11及之后保证线程安全的初始化方式）
GameObjectMgr GameObjectMgr::instance;


// 实现setObjectMap静态成员函数
void GameObjectMgr::setObjectMap(std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> map) {
    p_mapObject = map;
}

// 实现getObject静态成员函数
std::shared_ptr<GameObject> GameObjectMgr::getObject(UINT id) {
    if (p_mapObject) {
        auto it = p_mapObject->find(id);
        if (it != p_mapObject->end()) {
            return it->second;
        }
    }
    return nullptr;
}

// 实现addObject静态成员函数
void GameObjectMgr::addObject(UINT id, std::shared_ptr<GameObject> obj) {
    if (p_mapObject) {
        (*p_mapObject)[id] = obj;
    }
}