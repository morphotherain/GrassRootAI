
#pragma once
#include <fstream>
#include "UIWindow.h"
#include "UIText.h"
#include "UIButton.h"

class UIWindowStorage : public UIWindow
{
public:
	UIWindowStorage();
	UIWindowStorage(HINSTANCE _hInstance) :UIWindow(_hInstance) {}
	~UIWindowStorage() = default;

	struct Item {
		Item() = default;
		~Item() = default;

		int objectID;
		std::shared_ptr<UIButton> currentSelectBox;

	};

	struct Container {
		Container();
		~Container() = default;

		int ContainerID;
		int index = 0;
		int level = 0;
		std::shared_ptr<UIText> text;

		void Init();
		void Update();
		void Draw();
		std::unordered_map<int, std::shared_ptr<Container>> m_mapContainers;
	};

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	bool InitItemImgEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }

	virtual void ParseParameters(std::unordered_map<std::string, std::any> paras);

protected:
	void InitWindowComponent();


private:
	std::unordered_map<int,std::shared_ptr<Container>> m_mapOpenContainers;
	std::shared_ptr<Container> shipCargo;
	std::shared_ptr<Container> stationShipHangar;
	std::shared_ptr<Container> stationCargo;

	std::vector<std::pair<int, int>> m_itemsPair;
	std::vector<std::unique_ptr<UIText>> m_itemsTexts;


	std::shared_ptr<Effect> m_itemImgEffect;
	std::vector<std::shared_ptr<Effect>> m_itemImgEffects;
	std::shared_ptr<Effect> m_itemBackEffect;
	std::shared_ptr<Effect> m_itemNumBackEffect;


	std::shared_ptr<UIButton> currentContainerBox;
	std::shared_ptr<UIText> currentContainerTitle;
	std::shared_ptr<UIText> currentContainerUsed;
	int currentContainerID = 0;




};

