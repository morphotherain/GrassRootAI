#pragma once
#include <unordered_map>
#include <vector>
#include "GameObject.h"
#include "Effect.h"

class RenderProxy {
public:
    // 添加游戏对象和对应的效果到代理中
    void addGameObject(std::shared_ptr<GameObject> gameObject,std::shared_ptr<Effect> effect);

    // 执行渲染操作，遍历所有效果 - 游戏对象组并渲染
    void render();

private:
    std::unordered_map<std::shared_ptr<Effect>, std::shared_ptr<GameObject>> renderMap;
};

