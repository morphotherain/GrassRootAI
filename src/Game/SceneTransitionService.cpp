#include "SceneTransitionService.h"
#include "SolarSystemMgr.h"

std::optional<SceneId> SceneTransitionService::EvaluateTransition(const SceneTransitionContext& ctx)
{
	const bool needSwitch = ctx.shipSolarSystemID != ctx.currentSolarSystemID;
	if (ctx.tick % 100 != 0 && !needSwitch)
	{
		return std::nullopt;
	}

	while (true)
	{
		if (ctx.currentSceneId == SceneId::StargateLoading)
		{
			return SceneId::Space;
		}

		if (ctx.shipSolarSystemID != ctx.currentSolarSystemID)
		{
			std::optional<SceneId> loadingScene;
			if (ctx.currentSceneId != SceneId::StargateLoading)
			{
				loadingScene = SceneId::StargateLoading;
			}

			SolarSystemMgr::getInstance().switchToSolarSystem(ctx.shipSolarSystemID);
			return loadingScene;
		}

		if (ctx.containerID == 0)
		{
			if (ctx.currentSceneId != SceneId::Space)
			{
				return SceneId::Space;
			}
			break;
		}

		if (ctx.containerID != 0)
		{
			if (ctx.currentSceneId != SceneId::Dock)
			{
				return SceneId::Dock;
			}
			break;
		}

		break;
	}

	return std::nullopt;
}
