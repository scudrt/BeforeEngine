#include "BeforeEngine.h"
#include "GameTimer.h"

// The Global app of DirectX12
BeforeEngine GEngine;
GameTimer GGameTimer;

void updateFPSInfo(float FPS, float MSPF) {
	std::wstring fpsStr = std::to_wstring(FPS);
	std::wstring mspfStr = std::to_wstring(MSPF);
	std::wstring newTitleStr = L"D3D12Init FPS:" + fpsStr + L" | frame time: " + mspfStr + L" ms";
	SetWindowTextW(mhMainWnd, newTitleStr.c_str());
}

void updateFrameState() {
	static int frameCount = 0;
	static float lastSecond = 0.0f;

	++frameCount;

	if (GGameTimer.totalTime() - lastSecond >= 1.0f) {
		updateFPSInfo(frameCount, 1000.0f / (float)frameCount);
		lastSecond += 1.0f;
		frameCount = 0;
	}
}

/*
* The implementation of game loop
*/
void _gameLoop() {
	GGameTimer.tick();

	if (GGameTimer.isStopped()) {
		Sleep(500);
		return;
	}

	updateFrameState();

	GEngine.Render();
}

int _run() {
	//消息循环
	//定义消息结构体
	MSG msg = { 0 };
	BOOL bRet = 0;
	// Initialize game timer
	GGameTimer.reset();

	//如果GetMessage函数不等于0，说明没有接受到WM_QUIT
	while (msg.message != WM_QUIT) {
		// Message incoming
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			// Translate keyboard message into character messages
			TranslateMessage(&msg);
			// Distribute the message to the corresponding process
			DispatchMessage(&msg);
		}
		// No message, keep looping
		else {
			_gameLoop();
		}
	}
	return (int)msg.wParam;
}

// 窗口过程函数
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//消息处理
	switch (msg)
	{
		//当窗口被销毁时，终止消息循环
	case WM_DESTROY:
		PostQuitMessage(0);	//终止消息循环，并发出WM_QUIT消息
		return 0;
	default:
		break;
	}
	//将上面没有处理的消息转发给默认的窗口过程
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
bool initWindow(HINSTANCE hInstance, int nShowCmd) {
	//窗口初始化描述结构体(WNDCLASS)
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;	//当工作区宽高改变，则重新绘制窗口
	wc.lpfnWndProc = MainWndProc;	//指定窗口过程
	wc.cbClsExtra = 0;	//借助这两个字段来为当前应用分配额外的内存空间（这里不分配，所以置0）
	wc.cbWndExtra = 0;	//借助这两个字段来为当前应用分配额外的内存空间（这里不分配，所以置0）
	wc.hInstance = hInstance;	//应用程序实例句柄（由WinMain传入）
	wc.hIcon = LoadIcon(0, IDC_ARROW);	//使用默认的应用程序图标
	wc.hCursor = LoadCursor(0, IDC_ARROW);	//使用标准的鼠标指针样式
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//指定了白色背景画刷句柄
	wc.lpszMenuName = 0;	//没有菜单栏
	wc.lpszClassName = LPCSTR("MainWnd");	//窗口名
	//窗口类注册失败
	if (!RegisterClass(&wc))
	{
		//消息框函数，参数1：消息框所属窗口句柄，可为NULL。参数2：消息框显示的文本信息。参数3：标题文本。参数4：消息框样式
		MessageBox(0, LPSTR("RegisterClass Failed"), 0, 0);
		return false;
	}

	//窗口类注册成功
	RECT R;	//裁剪矩形
	R.left = 0;
	R.top = 0;
	R.right = 1280;
	R.bottom = 720;
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);	//根据窗口的客户区大小计算窗口的大小
	int width = R.right - R.left;
	int hight = R.bottom - R.top;

	//创建窗口,返回布尔值
	mhMainWnd = CreateWindow(LPCSTR("MainWnd"), LPCSTR("DX12Initialize"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, hight, 0, 0, hInstance, 0);
	//窗口创建失败
	if (!mhMainWnd)
	{
		MessageBox(0, LPCSTR("CreatWindow Failed"), 0, 0);
		return false;
	}
	//窗口创建成功,则显示并更新窗口
	ShowWindow(mhMainWnd, nShowCmd);
	UpdateWindow(mhMainWnd);

	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd) {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try {
		if (!initWindow(hInstance, nShowCmd)) {
			return 0;
		}
		if (!GEngine.InitGraphics()) {
			return 0;
		}

		return _run();
	}
	catch (DxException& e) {
		MessageBoxW(nullptr, e.ToString().c_str(), LPCWSTR("HR Failed"), MB_OK);
		return 0;
	}
}
