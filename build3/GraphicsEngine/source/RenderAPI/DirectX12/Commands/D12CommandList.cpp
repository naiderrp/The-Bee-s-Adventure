#include "pch.h"
#include "D12CommandList.h"

namespace GraphicsEngine {
	D12CommandList::~D12CommandList()
	{
		Release();
	}
	void D12CommandList::Initialize(ID3D12Device* pDevice)
	{
		//D3D12_COMMAND_LIST_TYPE_BUNDLE;
		API_EVAL_HR(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())), "error creating command allocator");
		API_EVAL_HR(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(GetAddressOf())), "error creating command list");
	}
	void D12CommandList::Release()
	{
		if (Get()) Reset();
		if (mCommandAllocator.Get()) mCommandAllocator.Reset();
	}
	void D12CommandList::ResetCommandList()
	{
		mCommandAllocator->Reset();
		GFXCmd()->Reset(mCommandAllocator.Get(), nullptr);
	}
}