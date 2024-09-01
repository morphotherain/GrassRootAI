#include "UIWindowInfo.h"
#include "UIButton.h"
#include "UIText.h"
#include "DatabaseManager.h"
#include <locale>
#include <codecvt>

void testQueryAndWriteToFile(int typeID);

std::wstring sqlite3_column_wstring(sqlite3_stmt* stmt, int column_index) {
	const char* text = (const char*)(sqlite3_column_text(stmt, column_index));
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(text);
}
std::string sqlite3_column_string(sqlite3_stmt* stmt, int column_index) {
	const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, column_index));
	return std::string(text);
}

// 提取不带后缀的文件名
std::string extractFilename(const std::string& path) {
	size_t lastSlash = path.find_last_of('/');
	if (lastSlash != std::string::npos) {
		std::string filename = path.substr(lastSlash + 1);
		size_t dotPos = filename.find(".");
		if (dotPos != std::string::npos) {
			return filename.substr(0, dotPos);
		}
		return filename;
	}
	return "";
}


// 去除宽字符串中的尖括号内容
std::wstring removeAngleBracketsContent(const std::wstring& str) {
	std::wstring result = str;
	size_t pos = 0;
	while ((pos = result.find(L"<", pos)) != std::wstring::npos) {
		size_t endPos = result.find(L">", pos);
		if (endPos != std::wstring::npos) {
			result.erase(pos, endPos - pos + 1);
		}
		else {
			break;
		}
		// 移动查找位置到下一个字符
		pos++;
	}
	return result;
}

std::wstring result = L"";
void TestDataBase() {
	sqlite3* db = DatabaseManager::getInstance()->getDatabase();
}

using namespace DirectX;


UIWindowInfo::UIWindowInfo() : UIWindow()
{

}

bool UIWindowInfo::Init()
{
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	std::string sql = "SELECT * FROM invtypes WHERE typeID = " + std::to_string(typeID) + ";";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

		testItem.Name = sqlite3_column_wstring(stmt, 6);
		testItem.Description = sqlite3_column_wstring(stmt, 12);
		testItem.IconId = sqlite3_column_int(stmt, 17);
	}

	testItem.Description = removeAngleBracketsContent(testItem.Description);

	std::string sq2 = "SELECT * FROM eveIcons WHERE iconID = " + std::to_string(testItem.IconId) + ";";
	sqlite3_finalize(stmt);

	sqlite3_stmt* stmt2;
	rc = sqlite3_prepare_v2(db, sq2.c_str(), -1, &stmt2, nullptr);

	while ((rc = sqlite3_step(stmt2)) == SQLITE_ROW) {
		testItem.IconPathPrint = sqlite3_column_wstring(stmt2, 1);
		testItem.IconPath = sqlite3_column_string(stmt2, 1);
		testItem.IconPath = extractFilename(testItem.IconPath);
	}
	sqlite3_finalize(stmt2);

	InitWindowComponent();

	auto button = std::make_shared<UIButton>();
	button->setSize(x+20.0f, y+160.f, 379.0f, 1.0f);
	button->setTex("demoTex\\UI\\Window\\window_line.dds");
	AddUIComponent(button);

	button = std::make_shared<UIButton>();
	button->setSize(x + 20.0f, y + 161.f, 50.0f, 25.0f);
	button->setTex("demoTex\\UI\\Window\\window_line.dds");
	AddUIComponent(button);


	auto text = std::make_shared<UIText>();
	text->setSize(x + 170.0f, y + 50.0f, 350.0f, 350.0f);
	text->setText(testItem.Name);
	AddUIComponent(text);

	std::wstring Description = L"描述";
	text = std::make_shared<UIText>();
	text->setSize(x + 30.0f, y + 164.0f, 350.0f, 350.0f);
	text->setText(Description);
	AddUIComponent(text);

	Description = L"属性";
	text = std::make_shared<UIText>();
	text->setSize(x + 80.0f, y + 164.0f, 350.0f, 350.0f);
	text->setText(Description);
	AddUIComponent(text);

	Description = L"物品:详情";
	text = std::make_shared<UIText>();
	text->setSize(x + 3.0f, y + 5.0f, 350.0f, 350.0f);
	text->setText(Description);
	AddUIComponent(text);


	text = std::make_shared<UIText>();
	text->setSize(x + 25.0f, y + 200.0f, 350.0f, 350.0f);
	text->setText(testItem.Description);
	AddUIComponent(text);


	std::string IconPath = "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\items\\dds\\";
	IconPath += testItem.IconPath;
	IconPath += ".dds";

	button = std::make_shared<UIButton>();
	button->setSize(x + 20.0f, y + 30.0f, 128.0f, 128.0f);
	button->setTex(IconPath);
	AddUIComponent(button);

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& component : childComponents) {
		component->setd3dResource(
			*m_pd3dDevice.GetAddressOf(),
			*m_pd3dImmediateContext.GetAddressOf(),
			*m_pSwapChain.GetAddressOf(),
			m_hMainWnd,
			*m_pRenderTargetView.GetAddressOf(),
			*m_pDepthStencilView.GetAddressOf()
		);
		component->setd2dResource(*m_pd2dRenderTarget.GetAddressOf(), *m_pColorBrush.GetAddressOf(), *m_pTextFormat.GetAddressOf());
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}


	return false;
}

void UIWindowInfo::OnResize()
{
}

void UIWindowInfo::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);




	// 在鼠标没进入窗口前仍为ABSOLUTE模式
	if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
	{
		if ((x * 10) < mouseState.x && ((x + deltaX) * 10) > mouseState.x && (1080 - y * 10 - deltaY * 10) < mouseState.y && (1080 - y * 10) > mouseState.y)
			switchScene = 2;
	}


}

void UIWindowInfo::DrawUI()
{

	for (auto& component : childComponents) {
		component->DrawUI();
	}

}

void UIWindowInfo::cleanup()
{
}



bool UIWindowInfo::InitResource()
{

	return true;
}

bool UIWindowInfo::InitEffect()
{

	return true;
}