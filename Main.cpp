#include "version.h"
#include "GameApp.h"
 

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance,
				   _In_ LPSTR cmdLine, _In_ int showCmd)
{
	// 这些参数不使用
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);
	UNREFERENCED_PARAMETER(showCmd);
	// 允许在Debug版本进行运行时内存分配和泄漏检测
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	
	// 创建控制台窗口
	AllocConsole();
	FILE* fp;
	if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0) {
		std::cerr << "Failed to redirect stdout to console." << std::endl;
	}
	if (freopen_s(&fp, "CONOUT$", "w", stderr) != 0) {
		std::cerr << "Failed to redirect stderr to console." << std::endl;
	}
	// 设置控制台输出为 UTF-8 编码

	spdlog::set_level(spdlog::level::debug);

	SetConsoleOutputCP(65001);


#endif


	

	initLogging();

	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] (+%-4o) [%^%l%$] %v");
	//spdlog::info("当前启动游戏版本：{}", GAME_VERSION);
	INFO_("当前启动游戏版本：{}", GAME_VERSION);
	

	GameApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;

	theApp.Run();

	shutdownLogging();
	// 释放控制台窗口
	FreeConsole();

	return 0;
}

/*spdlog::trace("这是一条跟踪级别的日志信息");
spdlog::debug("这是一条调试级别的日志信息");
spdlog::info("这是一条信息级别的日志信息");
spdlog::warn("这是一条警告级别的日志信息");
spdlog::error("这是一条错误级别的日志信息");
spdlog::critical("这是一条严重级别的日志信息");*/



