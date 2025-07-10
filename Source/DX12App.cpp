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

void DX12App::SetupSMAA() {
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
