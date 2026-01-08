#include "BeforeEngine.h"

#include <iostream>

#define DEBUG

void BeforeEngine::SetupModels() {
	/*
	* Definition of a box
	*/
	// Positions of vertices
	std::array<Vertex, 8> vertices = {
		Vertex({ vec3(-1.0f, -1.0f, -1.0f), vec4(1, 1, 1, 1) }),
		Vertex({ vec3(-1.0f, +1.0f, -1.0f), vec4(0, 0, 0, 1) }),
		Vertex({ vec3(+1.0f, +1.0f, -1.0f), vec4(1, 0, 0, 1) }),
		Vertex({ vec3(+1.0f, -1.0f, -1.0f), vec4(0, 1, 0, 1) }),
		Vertex({ vec3(-1.0f, -1.0f, +1.0f), vec4(0, 0, 1, 1) }),
		Vertex({ vec3(-1.0f, +1.0f, +1.0f), vec4(1, 1, 0, 1) }),
		Vertex({ vec3(+1.0f, +1.0f, +1.0f), vec4(0, 1, 1, 1) }),
		Vertex({ vec3(+1.0f, -1.0f, +1.0f), vec4(1, 0, 1, 1) })
	};
	// Indices of each face(triangle)
	std::array<int, 36> indices = {
		// front
		0, 1, 2,
		0, 2, 3,
		// back
		4, 6, 5,
		4, 7, 6,
		// left
		4, 5, 1,
		4, 1, 0,
		// right
		3, 2, 6,
		3, 6, 7,
		// up
		1, 5, 6,
		1, 6, 2,
		// down
		4, 0, 3,
		4, 3, 7
	};


	/*
	* Setting up vertex buffer
	*/
	// Create upload buffer and define a vertex buffer
	ComPtr<ID3D12Resource> vertexUploadBuffer = nullptr;
	ComPtr<ID3D12Resource> vertexBufferGPU = mGraphics.CreateDefaultBuffer(sizeof(vertices), vertices.data(), vertexUploadBuffer);
	// Bind vertex data to the render pipeline, indicating how to read vertex buffer
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	vertexBufferView.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(Vertex) * 8; // 8 vertex for a box
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	// Set vertex buffer to the graphics
	mGraphics.mCmdList->IASetVertexBuffers(0, 1, &vertexBufferView);

	/*
	* Setting up indices, as vertex buffer do
	*/
	ComPtr<ID3D12Resource> indexUploadBuffer = nullptr;
	ComPtr<ID3D12Resource> indexBufferGPU = mGraphics.CreateDefaultBuffer(sizeof(indices), indices.data(), indexUploadBuffer);
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(int) * indices.size();
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	mGraphics.mCmdList->IASetIndexBuffer(&indexBufferView);
}

ComPtr<ID3D12Resource> DX12Graphics::CreateDefaultBuffer(UINT64 byteSize, const void* data, ComPtr<ID3D12Resource>& uploadBuffer) {
	// Creating an upload buffer
	ThrowIfFailed(
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)
		)
	);

	// Creating a default buffer
	ComPtr<ID3D12Resource> defaultBuffer;
	ThrowIfFailed(
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&defaultBuffer)
		)
	);

	// Transfer default buffer state from COMMON to COPY_DEST
	mCmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST
		)
	);
	// Copy data from upload buffer to the default buffer
	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = data;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = byteSize;
	UpdateSubresources<1>(
		mCmdList.Get(),
		defaultBuffer.Get(),
		uploadBuffer.Get(),
		0, 0, 1,
		&subResourceData
	);

	// Transfer default buffer state from COPY_DEST to GENERIC_READ
	mCmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ
		)
	);

	return defaultBuffer;
}

void BeforeEngine::Render() {
	// Reset command objects firstly
	ThrowIfFailed(mGraphics.mCmdAllocator->Reset());
	ThrowIfFailed(mGraphics.mCmdList->Reset(mGraphics.mCmdAllocator.Get(), nullptr));

	// Swap buffer
	mGraphics.mCmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			mGraphics.mSwapChainBuffer[mGraphics.mCurrentBufferIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

	// Setup viewport and scissor
	mGraphics.mCmdList->RSSetViewports(1, &mGraphics.mViewport);
	mGraphics.mCmdList->RSSetScissorRects(1, &mGraphics.mScissorRect);

	// Get and clear buffers
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mGraphics.mRTVHeap->GetCPUDescriptorHandleForHeapStart(),
		mGraphics.mCurrentBufferIndex,
		mGraphics.mRTVDescSize
	);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mGraphics.mDSVHeap->GetCPUDescriptorHandleForHeapStart();
	mGraphics.mCmdList->ClearRenderTargetView(
		rtvHandle,
		DirectX::Colors::Blue,
		0, nullptr
	);
	mGraphics.mCmdList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0,
		0, nullptr
	);

	// Set render target ans depth stencil view
	mGraphics.mCmdList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
	// Wait for buffer to be presented
	mGraphics.mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		mGraphics.mSwapChainBuffer[mGraphics.mCurrentBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	));
	ThrowIfFailed(mGraphics.mCmdList->Close());

	mGraphics.SubmitCommandList();

	ThrowIfFailed(mGraphics.mSwapChain->Present(0, 0));
	mGraphics.mCurrentBufferIndex = (mGraphics.mCurrentBufferIndex + 1) & 1;

	mGraphics.FlushCmdQueue();
}

bool BeforeEngine::InitGraphics() {
#if defined(DEBUG) || defined(_DEBUG)
	// Enable debug layer if debug mode is enabled
	ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif
	
	// Go
	mGraphics.CreateDevice();
	mGraphics.CreateFence();
	mGraphics.GetDescriptorSizes();
	mGraphics.SetupSMAA();
	mGraphics.CreateCommandObjects();
	mGraphics.CreateSwapChain();
	mGraphics.CreateDescriptorHeap();
	mGraphics.CreateRTV();
	mGraphics.CreateDSV();
	mGraphics.CreateViewportAndScissorRect();

	return true;
}

void DX12Graphics::CreateDevice() {
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory)));

	ThrowIfFailed(
		D3D12CreateDevice(
			nullptr, // Use main device
			D3D_FEATURE_LEVEL_12_0, // The minimum feature level
			IID_PPV_ARGS(&mDevice) // Create and returned device
		)
	);
}

void DX12Graphics::CreateFence() {
	ThrowIfFailed(
		mDevice->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&mFence)
		)
	);
}

void DX12Graphics::GetDescriptorSizes() {
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
void DX12Graphics::SetupSMAA() {
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
void DX12Graphics::CreateCommandObjects() {
	// Create command queue using command queue description
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(
		mDevice->CreateCommandQueue(
			&commandQueueDesc,
			IID_PPV_ARGS(&mCmdQueue)
		)
	);

	// Create command allocator
	ThrowIfFailed(
		mDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&mCmdAllocator)
		)
	);

	// Create command list
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
void DX12Graphics::CreateSwapChain() {
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
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Flip and discard the old buffer
	swapChainDesc.BufferCount = 2; // Double buffers
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Auto-customizing

	// Try release swap chain before reallocating it
	mSwapChain.Reset();
	// Create swap chain, finally
	ThrowIfFailed(
		mDXGIFactory->CreateSwapChain(
			mCmdQueue.Get(), // Use cmdQueue instead of device
			&swapChainDesc,
			mSwapChain.GetAddressOf()
		)
	);
}

void DX12Graphics::CreateDescriptorHeap() {
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

void DX12Graphics::CreateRTV() {
	mCurrentBufferIndex = 0; // Use index 0 as the front buffer at the first frame

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

void DX12Graphics::CreateDSV() {
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
	mCmdList->Close();
	SubmitCommandList();
}

/*
* Synchronize and flush command queue from CPU to GPU
*/
void DX12Graphics::FlushCmdQueue() {
	++mCurrentFenceValue;
	mCmdQueue->Signal(mFence.Get(), mCurrentFenceValue);

	// Check if the current fence value is synced, if not, then wait for it
	if (mFence->GetCompletedValue() < mCurrentFenceValue) {
		// Create wait event
		HANDLE eventHandle = CreateEvent(nullptr, false, false, LPCSTR("FenceSetDone"));
		if (eventHandle == 0) {
			std::cerr << "DX12Graphics::FlushCmdQueue(): Failed to create event handle:" << eventHandle << std::endl;
			exit(1);
		}
		// Once the fence is synced to the latest, trigger this event
		mFence->SetEventOnCompletion(mCurrentFenceValue, eventHandle);
		// Wait until this event is triggered.
		WaitForSingleObject(eventHandle, INFINITE);
		// Don't forget to release the resource
		CloseHandle(eventHandle);
	}
}

/*
* Setup and create viewport and scissor rect for the pipeline
*/
void DX12Graphics::CreateViewportAndScissorRect() {
	// Setup viewport
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = 1280;
	mViewport.Height = 720;
	mViewport.MaxDepth = 1.0f;
	mViewport.MinDepth = 0.0f;

	// Setup scissor rect
	mScissorRect.left = 0;
	mScissorRect.top = 0;
	mScissorRect.right = 1280;
	mScissorRect.bottom = 720;
}

void DX12Graphics::SubmitCommandList() {
	// Construct command list array and pass it to the command queue for execution
	ID3D12CommandList* cmdLists[] = { mCmdList.Get() };
	mCmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}
