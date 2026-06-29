#include "MainMenuScreenController.h"
#include "UINavigationStack.h"
#include "SaveSlotListScreenController.h"
#include "SaveCreateScreenController.h"
#include <Windows.h>

MainMenuScreenController::MainMenuScreenController(UINavigationStack& navigation, UIGameplayHost host)
	: m_navigation(navigation)
	, m_host(std::move(host))
{
}

void MainMenuScreenController::OnShow(UISystem& system, UIRenderContext& context)
{
	(void)system;
	(void)context;

	RegisterAction("game.start_new", [this]() {
		if (m_host.onQuickStartNewGame)
		{
			m_host.onQuickStartNewGame();
		}
	});

	RegisterAction("nav.open.save_list", [this]() {
		m_navigation.Push(
			"resource/ui/save_slot_list.ui.json",
			std::make_unique<SaveSlotListScreenController>(m_navigation, m_host));
	});

	RegisterAction("nav.open.save_create", [this]() {
		m_navigation.Push(
			"resource/ui/save_create.ui.json",
			std::make_unique<SaveCreateScreenController>(m_navigation, m_host));
	});

	RegisterAction("app.quit", []() {
		PostQuitMessage(0);
	});
}
