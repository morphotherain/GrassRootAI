#pragma once

#include <functional>
#include <string>

// MainScene 注入、Screen 控制器只读的 gameplay 回调集合。
struct UIGameplayHost
{
	std::function<void()> onQuickStartNewGame;
	std::function<void(int slotId)> onEnterGameFromSlot;
	std::function<void(const std::string& displayName)> onCreateAndEnterGame;
	std::function<bool(int slotId)> onDeleteSaveSlot;
};
