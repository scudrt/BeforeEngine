#pragma once

#include "before_dx12.h"

class DX12App {
public:
	/*
	* Setup DX12 pipeline
	*/
	bool initDirectX12();

	/*
	* Render the scene
	*/
	void render();

	/*
	* Synchronize and flush command queue from CPU to GPU
	*/
	void FlushCmdQueue();
protected:
	/*
	* Create DXGI factory and device
	*/
	void createDevice();

	/*
	* Create fence for syncronizing GPU and CPU
	*/
	void createFence();

	/*
	* Get sizes for all descriptors
	*/
	void getDescriptorSizes();

	/*
	* Setup SMAA
	*/
	void setupSMAA();

	/*
	* Create command objects including command queue, command allocator and command list
	*/
	void createCommandObjects();

	/*
	* Create swap chain
	*/
	void createSwapChain();

	/*
	* Create descriptor heap
	*/
	void createDescriptorHeap();

	/*
	* Create render target view and depth stencil view
	*/
	void createRTV();
	void createDSV();

	/*
	* Setup and create viewport and scissor rect for the pipeline
	*/
	void CreateViewportAndScissorRect();
private:
	/*
	* ComPtrs for DX12 components
	*/
	ComPtr<IDXGIFactory4> mDXGIFactory;
	ComPtr<ID3D12Device> mDevice;
	ComPtr<ID3D12Fence> mFence;

	/*
	* ComPtrs for command objects
	*/
	ComPtr<ID3D12CommandQueue> mCmdQueue;
	ComPtr<ID3D12CommandAllocator> mCmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> mCmdList; // Attention it is a graphics command list

	/*
	* Comptr for swap chain
	*/
	ComPtr<IDXGISwapChain> mSwapChain;

	/*
	* Descriptor heaps and resources
	*/
	ComPtr<ID3D12DescriptorHeap> mRTVHeap; // Render target descriptor heap
	ComPtr<ID3D12DescriptorHeap> mDSVHeap; // Depth stencil descriptor heap
	ComPtr<ID3D12Resource> mSwapChainBuffer[2]; // Buffers for RTV
	ComPtr<ID3D12Resource> mDepthStencilBuffer; // Buffer for DSV


	// Size of render target view
	UINT mRTVDescSize;
	// Size of depth stencil view
	UINT mDSVDescSize;
	// Size of const buffer view(CBV) | shader resource view(SRV) and random access view(RAV)
	UINT mCBV_SRV_UAVDescSize;
	// Indicating the current back buffer index for buffer swapping
	UINT mCurrentBufferIndex;

	int mCurrentFenceValue;

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS mMSAAQualityLevels;
};
