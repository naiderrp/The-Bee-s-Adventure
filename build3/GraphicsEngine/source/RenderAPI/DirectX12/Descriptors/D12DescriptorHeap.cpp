#include "pch.h"
#include "D12DescriptorHeap.h"

namespace GraphicsEngine {
	D12DescriptorHeap::~D12DescriptorHeap()
	{
		Release();
	}
	void D12DescriptorHeap::InitializeAsDepthHeap(ID3D12Device* pDevice, UINT width, UINT height)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = 1;
		desc.NodeMask = 0;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		API_EVAL_HR(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(GetAddressOf())), "Error creating descriptor heap");
	}
	void D12DescriptorHeap::Release()
	{
		if (Get()) Reset();
	}
}