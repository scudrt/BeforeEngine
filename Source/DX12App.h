#pragma once

#include "before_dx12.h"

class DX12App {
public:
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
	void SetupSMAA();
protected:
	;
private:
	/*
	* ComPtrs for DX12 components
	*/
	ComPtr<IDXGIFactory4> mDXGIFactory;
	ComPtr<ID3D12Device> mDevice;
	ComPtr<ID3D12Fence> mFence;

	// Size of render target view
	UINT mRTVDescSize;
	// Size of depth stencil view
	UINT mDSVDescSize;
	// Size of const buffer view(CBV) | shader resource view(SRV) and random access view(RAV)
	UINT mCBV_SRV_UAVDescSize;

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS mMSAAQualityLevels;
};
