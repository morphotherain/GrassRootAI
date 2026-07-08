#include "SceneTransitionService.h"
#include "SolarSystemMgr.h"
#include "SimLog.h"

namespace
{
	const char* SceneIdName(SceneId id)
	{
		switch (id) {
		case SceneId::Main: return "Main";
		case SceneId::Dock: return "Dock";
		case SceneId::Space: return "Space";
		case SceneId::StargateLoading: return "StargateLoading";
		default: return "Unknown";
		}
	}
}

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
			LOG_SCENE("切换 {} -> Space (星门加载完成)", SceneIdName(ctx.currentSceneId));
			return SceneId::Space;
		}

		if (ctx.shipSolarSystemID != ctx.currentSolarSystemID)
		{
			std::optional<SceneId> loadingScene;
			if (ctx.currentSceneId != SceneId::StargateLoading)
			{
				loadingScene = SceneId::StargateLoading;
				LOG_SCENE("切换 {} -> StargateLoading (跨星系 {} -> {})",
					SceneIdName(ctx.currentSceneId), ctx.currentSolarSystemID, ctx.shipSolarSystemID);
			}

			SolarSystemMgr::getInstance().switchToSolarSystem(ctx.shipSolarSystemID);
			return loadingScene;
		}

		if (ctx.shipLocationKind == AssetLocationKind::SolarSystem)
		{
			if (ctx.currentSceneId != SceneId::Space)
			{
				LOG_SCENE("切换 {} -> Space (出站)", SceneIdName(ctx.currentSceneId));
				return SceneId::Space;
			}
			break;
		}

		if (ctx.shipLocationKind == AssetLocationKind::DockedAtStructure)
		{
			if (ctx.currentSceneId != SceneId::Dock)
			{
				LOG_SCENE("切换 {} -> Dock (进站 anchor={})", SceneIdName(ctx.currentSceneId), ctx.locationRef);
				return SceneId::Dock;
			}
			break;
		}

		break;
	}

	return std::nullopt;
}
