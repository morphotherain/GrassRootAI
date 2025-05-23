﻿#include "UIWindowInfo.h"
#include "UIButton.h"
#include "UIText.h"
#include "DatabaseManager.h"
#include <locale>
#include <codecvt>

void testQueryAndWriteToFile(int typeID);

std::wstring sqlite3_column_wstring(sqlite3_stmt* stmt, int column_index) {
	return DatabaseManager::getInstance()->sqlite3_column_wstring(stmt, column_index);
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
	UIWindow::UIWindow();
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

	auto text = std::make_shared<UIText>();
	text->setSize(170.0f, 50.0f, 350.0f, 350.0f);
	text->setText(testItem.Name);
	AddUIComponent(text);

	std::wstring Description = L"描述";
	text = std::make_shared<UIText>();
	text->setSize(30.0f, 164.0f, 350.0f, 350.0f);
	text->setText(Description);
	AddUIComponent(text);

	Description = L"属性";
	text = std::make_shared<UIText>();
	text->setSize(80.0f, 164.0f, 350.0f, 350.0f);
	text->setText(Description);
	AddUIComponent(text);

	Description = L"物品:详情";
	text = std::make_shared<UIText>();
	text->setSize(3.0f, 5.0f, 350.0f, 350.0f);
	text->setText(Description);
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(25.0f, 200.0f, 350.0f, 350.0f);
	text->setText(testItem.Description);
	AddUIComponent(text);

	std::string IconPath = "demoTex\\EVE\\media\\res\\Uprising_V21.03_Types\\Types\\dds\\";
	IconPath += std::to_string(typeID);//testItem.IconPath;
	IconPath += "_64.dds";

	InitWindowComponent();
	auto Texture = m_windowEffect->getTextures();
	Texture->addTextureFileName("demoTex\\UI\\Window\\window_line.dds");

	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertices, 20.0f, 160.f, 379.0f, 1.0f, 5.0f);
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertices);
	m_windowEffect->Init();

	m_itemImgEffect = std::make_shared<Effect>();
	std::vector<PosTexIndex> vertexsImg = {};
	GenerateRectVertex(vertexsImg, 20.0f, 30.0f, 128.0f, 128.0f, 6.0f);
	m_itemImgEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_itemImgEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsImg);
	m_itemImgEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_itemImgEffect->addTextures({ IconPath });
	m_itemImgEffect->addConstantBuffer<ConstantMVPIndex>();
	m_itemImgEffect->Init();

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& component : childComponents) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}

	return false;
}

void UIWindowInfo::OnResize()
{
}

void UIWindowInfo::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	UIWindow::UpdateUI(dt, mouse, keyboard, tick);
	for (auto& component : childComponents) {
		component->setDelta(x, y);
	}
	if (!IsActive()) {
		return;
	}
}

void UIWindowInfo::DrawUI()
{
	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();

	XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);

	ConstantMVPIndex* dataPtr = m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_windowEffect->apply();

	ConstantMVPIndex* dataPtrImg = m_itemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtrImg->model = XMMatrixTranspose(windowModel);
	dataPtrImg->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtrImg->projection = XMMatrixTranspose(projMatrix);
	dataPtrImg->TexIndex = 0;
	m_itemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_itemImgEffect->apply();

	for (auto& component : childComponents) {
		component->DrawUI();
	}
}

void UIWindowInfo::cleanup()
{
}

void UIWindowInfo::ParseParameters(std::unordered_map<std::string, std::any> paras)
{
	typeID = getParameter<int>(paras, "typeID", -1);
	x = getParameter<float>(paras, "x", -1);
	SetZOrder(getParameter<int>(paras, "z_order", -1));

	y = 400;
	width = 400;
	height = 600;
}

bool UIWindowInfo::InitResource()
{
	return true;
}

bool UIWindowInfo::InitEffect()
{
	return true;
}