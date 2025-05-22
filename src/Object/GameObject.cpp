#include "GameObject.h"



void GameObject::ResolveDependencies()
{
    DEBUG_("正在解析所有组件的依赖关系");
    for (const auto& component : components) {
        auto dependencies = component->GetDependencies();
        DEBUG_("正在解析类型为 {} 的组件的依赖关系", typeid(*component).name());
        for (const auto& depType : dependencies) {
            for (const auto& depComponent : components) {
                if (typeid(*depComponent).hash_code() == depType.hash_code()) {
                    DEBUG_("将类型为 {} 的依赖项注入到类型为 {} 的组件中", depType.name(), typeid(*component).name());
                    component->InjectDependency(depComponent);
                }
            }
        }
    }
}
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

