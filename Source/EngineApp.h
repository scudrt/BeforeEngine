#pragma once

#include "BeforeRHI.h"
#include "GameTime.h"
#include <comdef.h>

class BeforeEngineApp {
public:
	BeforeEngineApp();

	bool init(HINSTANCE hInstance, int nShowCmd);

	int run();

	virtual void draw();

protected:
	void updateFrameState();

	bool __initWindow(HINSTANCE hInstance, int nShowCmd);
	bool __initRHI();

private:
	HWND mhMainWnd = 0;	// handle of the engine window
	RHIBase* mRHIBase = nullptr;
	GameTime mGameTime; // Global timer
};
