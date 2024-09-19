#include "pch.h"
#include "D12CommandQueue.h"

namespace GraphicsEngine {
	D12CommandQueue::~D12CommandQueue()
	{
		Release();
	}
	void D12CommandQueue::Initialize(ID3D12Device* pDevice)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;

		API_EVAL_HR(pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(GetAddressOf())), "Error creating command queue");
		API_EVAL_HR(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())), "Error creating the fence");
	}
	void D12CommandQueue::Release()
	{
		if (Get()) Reset();
		if (mFence.Get()) mFence.Reset();
	}
	void D12CommandQueue::ExecuteCommandList(ID3D12CommandList* pCommandList)
	{
		Get()->ExecuteCommandLists(1, (ID3D12CommandList* const*)&pCommandList);
		++mCurrentFenceValue;
		Get()->Signal(mFence.Get(), mCurrentFenceValue);
	}
	void D12CommandQueue::FlushQueue()
	{
		if (!Get()) return;

		for (int i = 0; i < MAX_SWAPCHAIN_BUFFERS; ++i)
			Get()->Signal(mFence.Get(), mCurrentFenceValue + i);
	}
}