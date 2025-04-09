#include "EngineApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd) {
#if defined(DEBUG) | defined(_DEBUG)
    #ifdef RHI_DX12 // enable debug layer for D3D12
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif
#endif

    try {
        BeforeEngineApp app;
        if (!app.init(hInstance, nShowCmd)) {
            return 0;
        }

        return app.run();
    }
    catch (DxException& e) {
        MessageBoxW(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}
