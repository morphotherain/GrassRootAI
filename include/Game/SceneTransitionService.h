#pragma once

#include "SceneId.h"
#include <optional>
#include <cstdint>

struct SceneTransitionContext
{
	SceneId currentSceneId = SceneId::None;
	std::uint32_t tick = 0;
	std::uint32_t containerID = 0;
	std::uint32_t shipSolarSystemID = 0;
	std::uint32_t currentSolarSystemID = 0;
};

class SceneTransitionService
{
public:
	static std::optional<SceneId> EvaluateTransition(const SceneTransitionContext& ctx);
};
