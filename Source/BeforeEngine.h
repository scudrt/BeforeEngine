#pragma once

#include "DX12Util.h"

/*****************************************************
* Math Type Aliasing
*****************************************************/
using vec2	= DirectX::XMFLOAT2;
using vec3	= DirectX::XMFLOAT3;
using vec4	= DirectX::XMFLOAT4;

using mat4	= DirectX::XMFLOAT4X4;
using mat43 = DirectX::XMFLOAT4X3;
using mat3	= DirectX::XMFLOAT3X3;

struct Vertex {
	vec3 mPosition;
	vec4 mColor;
};

struct ObjectConstants {
	mat4 worldViewProj;
};

/*****************************************************
* Implementation of some Tool Classes
*****************************************************/

template<typename ElementType>
class UploadBufferResource {
public:
	UploadBufferResource(ComPtr<ID3D12Device> device, UINT elementCount, bool isConstantBuffer):
		mData(nullptr),
		mElementCount(elementCount),
		mElementByteSize(sizeof(ElementType)),
		mIsConstantBuffer(isConstantBuffer) {

		// Constant buffer size must be the multiplier of 128
		if (isConstantBuffer) {
			mElementByteSize = (mElementByteSize + 127) & (~127);
		}

		ThrowIfFailed(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mUploadBuffer)
			)
		);
		mUploadBuffer->Map(
			0,
			nullptr, // Map the entire buffer
			ThrowIfFailed(reinterpret_cast<void**>(&mData))
		)
	}

	~UploadBufferResource() {
		// Release upload buffer resource
		if (mUploadBuffer != nullptr) {
			mUploadBuffer->Unmap(0, nullptr);

			mData = nullptr;
		}
	}

	ComPtr<ID3D12Resource> mUploadBuffer;
	void* mData = nullptr;
	UINT mElementCount;
	UINT mElementByteSize;
	bool mIsConstantBuffer;
};


/*****************************************************
* Implementation of DirectX 12 Graphics Layer
*****************************************************/

struct DX12Graphics {
public:
	/*
	* Synchronize and flush command queue from CPU to GPU
	*/
	void FlushCmdQueue();

	/*
	* Create DXGI factory and device
	*/
	void CreateDevice();

	/*
	* Create fence for syncronizing GPU and CPU
	*/
	void CreateFence();

	/*
	* Get sizes for all descriptors
	*/
	void GetDescriptorSizes();

	/*
	* Setup SMAA
	*/
	void SetupSMAA();

	/*
	* Create command objects including command queue, command allocator and command list
	*/
	void CreateCommandObjects();

	/*
	* Create swap chain
	*/
	void CreateSwapChain();

	/*
	* Create descriptor heap
	*/
	void CreateDescriptorHeap();

	/*
	* Create render target view and depth stencil view
	*/
	void CreateRTV();
	void CreateDSV();

	/*
	* Setup and create viewport and scissor rect for the pipeline
	*/
	void CreateViewportAndScissorRect();

	inline void SubmitCommandList();

	/*
	* Function for creating default buffer
	*/
	ComPtr<ID3D12Resource> CreateDefaultBuffer(UINT64 byteSize, const void* data, ComPtr<ID3D12Resource>& resultBuffer);

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

	/*
	* Viewport and scissor
	*/
	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

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

/*****************************************************
* Implementation of BeforeEngine Class
*****************************************************/

class BeforeEngine {
public:
	/*
	* Setup DX12 pipeline
	*/
	bool InitGraphics();

	/*
	* Render the scene
	*/
	void Render();

	void SetupModels();
private:
	DX12Graphics mGraphics;
};
