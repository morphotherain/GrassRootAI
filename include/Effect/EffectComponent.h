#pragma once
#include <memory>
#include "ddsLoader.h"
#include "vertexData.h"
#include <vector>

class EffectComponent {
public:
	virtual ~EffectComponent() {}

	virtual void Init() = 0;

	// 渲染组件（如果需要）
	virtual void apply() = 0;

	virtual void update() {};

	virtual void clear() {};
};