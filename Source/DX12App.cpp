#include "DX12App.h"

void DX12App::createDevice() {
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory)));

	ThrowIfFailed(
		D3D12CreateDevice(
			nullptr, // Use main device
			D3D_FEATURE_LEVEL_12_0, // The minimum feature level
			IID_PPV_ARGS(&mDevice) // Create and returned device
		)
	);
}

void DX12App::createFence() {
	ThrowIfFailed(
		mDevice->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&mFence)
		)
	);
}

void DX12App::getDescriptorSizes() {
	mRTVDescSize = mDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	);
	mDSVDescSize = mDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	);
	mCBV_SRV_UAVDescSize = mDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);
}

/*
* Setup SMAA
*/
void DX12App::setupSMAA() {
	mMSAAQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mMSAAQualityLevels.SampleCount = 1;
	mMSAAQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	mMSAAQualityLevels.NumQualityLevels = 0;

	// Check DX12 feature support to MSAA
	ThrowIfFailed(
		mDevice->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&mMSAAQualityLevels,
			sizeof(mMSAAQualityLevels)
		)
	);
	// if MSAA is supported, then NumQualityLevels will be set > 0
	assert(mMSAAQualityLevels.NumQualityLevels > 0);
}

/*
* Create command objects including command queue, command allocator and command list
*/
void DX12App::createCommandObjects() {
	// Create command queue using command queue description
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ComPtr<ID3D12CommandQueue> mCmdQueue;
	ThrowIfFailed(
		mDevice->CreateCommandQueue(
			&commandQueueDesc,
			IID_PPV_ARGS(&mCmdQueue)
		)
	);

	// Create command allocator
	ComPtr<ID3D12CommandAllocator> mCmdAllocator;
	ThrowIfFailed(
		mDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&mCmdAllocator)
		)
	);

	// Create command list
	ComPtr<ID3D12GraphicsCommandList> mCmdList;
	ThrowIfFailed(
		mDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			mCmdAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(&mCmdList)
		)
	);
	mCmdList->Close(); // Command list must be closed before resetting
}

/*
* Create swap chain
*/
void DX12App::createSwapChain() {
	// Try release swap chain before reallocating it
	mSwapChain.Reset();

	// Swap chain creating description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = 1280;
	swapChainDesc.BufferDesc.Height = 720;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use this buffer for render output
	swapChainDesc.OutputWindow = mhMainWnd; // Handle of output window
	swapChainDesc.SampleDesc.Count = 1; // The count/size of multisampling
	swapChainDesc.SampleDesc.Quality = 0; // The quality of multisampling
	swapChainDesc.BufferCount = 2; // Double buffers
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Flip and discard the old buffer
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Auto-customizing
	// Create swap chain, finally
	ThrowIfFailed(
		mDXGIFactory->CreateSwapChain(
			mDevice.Get(),
			&swapChainDesc,
			mSwapChain.GetAddressOf()
		)
	);
}

void DX12App::createDescriptorHeap() {
	// Create render target view descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.NumDescriptors = 2; // Double buffer
	rtvDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		mDevice->CreateDescriptorHeap(
			&rtvDescriptorHeapDesc,
			IID_PPV_ARGS(&mRTVHeap)
		)
	);

	// Create depth stencil view descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.NodeMask = 0;
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	ThrowIfFailed(
		mDevice->CreateDescriptorHeap(
			&dsvDescriptorHeapDesc,
			IID_PPV_ARGS(&mDSVHeap)
		)
	);
}


void DX12App::createRTV() {
	// Get descriptor handle from heap
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());
	// Create RTV by the descriptor handle
	for (int i = 0; i < 2; ++i) {
		// Bind swapchain buffer to the resource
		mSwapChain->GetBuffer(i, IID_PPV_ARGS(mSwapChainBuffer[i].GetAddressOf()));
		// Create RTV on the resource
		mDevice->CreateRenderTargetView(
			mSwapChainBuffer[i].Get(),
			nullptr,
			rtvHeapHandle
		);
		// Offset to the next swapchain buffer
		rtvHeapHandle.Offset(1, mRTVDescSize);
	}
}

void DX12App::createDSV() {
	D3D12_RESOURCE_DESC dsvResourceDesc;
	dsvResourceDesc.Alignment = 0;
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsvResourceDesc.DepthOrArraySize = 1;
	dsvResourceDesc.Width = 1280;
	dsvResourceDesc.Height = 720;
	dsvResourceDesc.MipLevels = 1;
	dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // Unspecified
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // The flag for depth stencil
	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
	dsvResourceDesc.SampleDesc.Count = 4;
	dsvResourceDesc.SampleDesc.Quality = mMSAAQualityLevels.NumQualityLevels - 1;

	// Optimized clear value for DSV
	CD3DX12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Corresponding to the format of DSV
	optClear.DepthStencil.Depth = 1; // Clear value of depth
	optClear.DepthStencil.Stencil = 0; // Clear value of stencil

	ThrowIfFailed(
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&dsvResourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(&mDepthStencilBuffer)
		)
	);
	// This function doesn't return anything, so we don't use ThrowIfFailed
	mDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(),
		nullptr, // We have specified resource desc in CreateCommittedResource, so we can set this nullptr
		mDSVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	// Add a command for transiting write state of DSV
	mCmdList->ResourceBarrier(
		1, // Num of barriers
		&CD3DX12_RESOURCE_BARRIER::Transition(
			mDepthStencilBuffer.Get(), // Resource to be transited
			D3D12_RESOURCE_STATE_COMMON, // Current state of the resource
			D3D12_RESOURCE_STATE_DEPTH_WRITE // Target state of the resource
		)
	);
	// End of the command list
	ThrowIfFailed(mCmdList->Close());
	// Construct command list array and pass it to the command queue for execution
	ID3D12CommandList* cmdLists[] = { mCmdList.Get() };
	mCmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}


/*
* Synchronize and flush command queue from CPU to GPU
*/
void DX12App::FlushCmdQueue() {
	// TODO
}
