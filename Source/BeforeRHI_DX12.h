#pragma once

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

class BeforeD3D12 {
public:
	BeforeD3D(HWND hwnd): windowHandle(hwnd){}

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

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Resource> swapChainBuffer[2]; // double-buffer
	ComPtr<ID3D12Resource> depthStencilBuffer;

	int currentFence = 0;
	UINT rtvDescriptorSize; // render target view descriptor size
	UINT dsvDescriptorSize; // depth stancil view descriptor size
	UINT cbv_src_uavDescriptorSize; // constant buffer view, shader resource cache, universal random access
};
