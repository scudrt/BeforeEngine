#include "BeforeRHI_DX12.h"

void BeforeD3D12::createDevice() {
	ThrowIfFailed(CreateDXGIFactory1(IID_PPVA_ARGS(&this->dxgiFactory)));
	ThrowIfFailed(
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&this->d3dDevice))
	);
}

void BeforeD3D12::createFence() {
	ThrowIfFailed(
		this->d3dDevice->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(this->fence);
		)
	)
}

void BeforeD3D12::getDescriptionSize() {
	rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	cbv_src_uavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void BeforeD3D12::setSMAA() {
	msaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msaaQualityLevels.SampleCount = 1;
	msaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msaaQualityLevels.NumQualityLevels = 0;
	// check current graphics driver's support for MSAA
	ThrowIfFailed(
		d3dDevice->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msaaQualityLevels,
			sizeof(msaaQualityLevels)
		)
	);
	// if MSAA if supported, NumQualityLevels > 0
	assert(msaaQualityLevels.NumQualityLevels > 0);
}

void BeforeD3D12::createCommandObjects() {
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(
		d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
	);
	ThrowIfFailed(
		d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&cmdAllocator) // &cmdAllocator == cmdAllocator.GetAddressOf()
		)
	);
	ThrowIfFailed(
		d3dDevice->CreateCommandList(
			0, // single gpu, mask 0
			D3D12_COMMAND_LIST_TYPE_DIRECT, // command list type
			this->cmdAllocator.Get(),
			nullptr, // 流水线状态对象PSO
			IID_PPV_ARGS(&graphicsCommandList)
		)
	);
	graphicsCommandList->Close(); // close before resetting command list
}

void BeforeD3D12::createSwapChain() {
	swapChain.Reset();
	DXGI_SWAP_CHAIN_DESC swapChainDesc; // description of swap chain
	swapChainDesc.BufferDesc.Width = 1280;
	swapChainDesc.BufferDesc.Height = 720;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1; // num of multisampling
	swapChainDesc.SampleDesc.Quality = 0; // quality of multisampling
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.BufferCount = 2; // double-buffer rendering
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // adaptive windowed mode
	ThrowIfFailed(
		dxgiFactory->CreateSwapChain(
			cmdQueue.Get(), // not device pointer, it is the mistake of this interface
			&swapChainDesc,
			swapChain.GetAddressOf()
		)
	);
}

void BeforeD3D12::createDescriptorHeap() {
	// Creating RTV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
	rtvDescriptorHeapDesc.NumDescriptors = 2; // two descriptors for double-buffer
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		d3dDevice->CreateDescriptorHeap(
			&rtvDescriptorHeapDesc,
			IID_PPV_ARGS(&rtvHeap)
		)
	);

	// Creating DSV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		d3dDevice->CreateDescriptorHeap(
			&dsvDescriptorHeapDesc,
			IID_PPV_ARGS(&dsvHeap)
		);
	);
}

void BeforeD3D12::createRTV() {
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i <= 2; ++i) {
		swapChain->GetBuffer(i, IID_PPV_ARGS(swapChainBuffer[i].GetAddressOf()));
		d3dDevice->CreateRenderTargetView(
			swapChainBuffer[i].Get(),
			nullptr,
			rtvHeapHandle
		);
		rtvHeapHandle.Offset(1, rtvDescriptorSize);
	}
}

void BeforeD3D12::createDSV() {
	D3D12_RESOURCE_DESC dsvResourceDesc;
	dsvResourceDesc.Alignment = 0;
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsvResourceDesc.DepthOrArraySize = 1; // texture depth
	dsvResourceDesc.Width = 1280;
	dsvResourceDesc.Height = 720;
	dsvResourceDesc.MipLevels = 1;
	dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // no specific here
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 for depth, 8 for stencil
	dsvResourceDesc.SampleDesc.Count = 4; // number of MSAA
	dsvResourceDesc.SampleDesc.Quality = msaaQualityLevels.NumQualityLevels - 1;
	
	// DSV value clearing setting
	CD3DX12_CLEAR_VALUE optClear; // optimized clear value
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 for depth and 8 for stencil
	optClear.DepthStencil.Depth = 1; // initial depth value
	optClear.DepthStencil.Stencil = 0; // initial stencil value
	
	ThrowIfFailed(
		d3dDevice->CreateCommittedResource(
			&dsvResourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(&depthStencilBuffer)
		)
	);

	d3dDevice->CreateDepthStencilView(
		depthStencilBuffer.Get(),
		nullptr,
		dsvHeap->GetCPUDescriptorHandleForHeapStart() // dsv handle
	);

	// marking DSV state
	graphicsCommandList->ResourceBarrier(
		1, // num of barriers
		&CD3DX12_RESOURCE_BARRIER::Transition(
			depthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, // state before transition
			D3D12_RESOURCE_STATE_DEPTH_WRITE // state after transition
		)
	);
	ThrowIfFailed(graphicsCommandList->Close());
	ID3D12CommandList* cmdLists[] = { graphicsCommandList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), graphicsCommandList); // committing commands from list to queue
}

void BeforeD3D12::flushCmdQueue() {
	++currentFence;
	cmdQueue->Signal(fence.Get(), currentFence);
	if (fence->GetCompletedValue() < currentFence) { // GPU fence < CPU fence
		/* GPU doesn't finish all commands */
		HANDLE handle = CreateEvent(nullptr, false, false, L"FenceSetDone");
		fence->SetEventOnCompletion(currentFence, handle); // trigger when GPU fence reach currentFence
		WaitForSingleObject(handle, INFINITE);
		CloseHandle(handle);
	}
}

void BeforeD3D12::createViewportAndScissorRect() {
	;
}
