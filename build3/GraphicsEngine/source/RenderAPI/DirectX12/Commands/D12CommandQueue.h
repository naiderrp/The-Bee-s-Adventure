#pragma once
#include <wrl.h>
#include "EngineMin.h"

namespace GraphicsEngine {
	class GRAPHICS_API D12CommandQueue : public Microsoft::WRL::ComPtr<ID3D12CommandQueue>
	{
	public:
		~D12CommandQueue();

		void Initialize(ID3D12Device* pDevice);
		void Release();

		void ExecuteCommandList(ID3D12CommandList* pCommandList);

		// Queue.WaitIdle()
		void FlushQueue();

		inline ID3D12Fence* GetFence() { return mFence.Get(); }
		inline UINT64 GetCurrentFenceValue() { return mCurrentFenceValue; }

	public:
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFenceValue = 0;
	};
}

