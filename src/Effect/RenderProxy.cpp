#include "RenderProxy.h"

//void RenderProxy::addGameObject(GameObject* gameObject, Effect* effect) {
//    renderMap[effect].push_back(gameObject);
//
//}
//
//void RenderProxy::render() {
//    for (auto& pair : renderMap) {
//        Effect* effect = pair.first;
//        std::vector<GameObject*>& gameObjects = pair.second;
//        // 应用效果
//        effect->apply();
//        for (auto gameObject : gameObjects) {
//            // 传递游戏对象相关参数给效果组件（这里需要根据实际需求实现）
//            // 例如：effect->setGameObjectParams(gameObject);
//            // 执行游戏对象的渲染提交（假设游戏对象有此方法）
//            //gameObject->submitToRender();
//        }
//    }
//}