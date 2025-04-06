#pragma once

#include "RHIBase.h"

#include <Windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <windowsx.h>
#include "d3dx12_utils/d3dx12.h"
#include <comdef.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#include "DxException.h"

class BeforeD3D12 : public RHIBase{
public:
	BeforeD3D12(HWND hwnd): windowHandle(hwnd){}

	virtual bool init() override;
	virtual void draw() override;
	virtual void onDestroy() override;

	void createDevice();

	void createFence();

	void getDescriptionSize();

	void setSMAA();

	void createCommandObjects();

	void createSwapChain();

	void createDescriptorHeap();

	void createRTV();

	void createDSV();

	void flushCmdQueue();

	void createViewportAndScissorRect();
protected:
	HWND windowHandle;
	ComPtr<IDXGIFactory4> dxgiFactory;
	ComPtr<ID3D12Device> d3dDevice;
	ComPtr<ID3D12Fence> fence;

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels = {};
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Resource> swapChainBuffer[2]; // double-buffer
	ComPtr<ID3D12Resource> depthStencilBuffer;

	int currentFence = 0;
	UINT curBackBuffer = 0;
	UINT rtvDescriptorSize = 0; // render target view descriptor size
	UINT dsvDescriptorSize = 0; // depth stancil view descriptor size
	UINT cbv_src_uavDescriptorSize = 0; // constant buffer view, shader resource cache, universal random access
};
