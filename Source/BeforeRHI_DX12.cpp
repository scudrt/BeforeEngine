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
