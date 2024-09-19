#pragma once
#include "RenderAPI/DirectX12/Resource/D12Resource.h"
#include "RenderAPI/DirectX12/Commands/D12CommandList.h"
#include "RenderAPI/DirectX12/Commands/D12CommandQueue.h"

namespace GraphicsEngine {
	class GRAPHICS_API D12BufferUploader
	{
	public:
		~D12BufferUploader();

		void Initialize(ID3D12Device* pDevice, const UINT memoryallocation);

		void Upload(D12Resource* pDestination, void* pSourceData, const UINT size, D12CommandList* pCommandList, D12CommandQueue* pCommandQueue, D3D12_RESOURCE_STATES customState = D3D12_RESOURCE_STATE_COMMON);

		void Release();

	private:
		D12Resource mInternalBuffer;
		void* mInternalMemory = nullptr;

		UINT mMaxSize = 0;
	};
}