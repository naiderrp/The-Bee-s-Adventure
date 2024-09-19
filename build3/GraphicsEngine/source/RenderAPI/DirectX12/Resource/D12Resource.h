#pragma once
#include <wrl.h>
#include "EngineMin.h"
#include <vector>

#include "Utilities/d3dx12.h"

namespace GraphicsEngine {
	class GRAPHICS_API D12Resource : public Microsoft::WRL::ComPtr<ID3D12Resource>
	{
	public:
		enum ShaderResourceType {
			eTextureCube = 0,
		};
	public:
		D12Resource() = default;

		D12Resource(const D12Resource& other) = default;
		D12Resource& operator=(const D12Resource& other) = default;

		D12Resource(D12Resource&& other) noexcept;
		D12Resource& operator=(D12Resource&& other) noexcept;

		~D12Resource();

		void Initialize(ID3D12Device* pDevice, UINT numBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState);
		void Release();

		void InitializeAsDepthBuffer(ID3D12Device* pDevice, UINT width, UINT height);

		void* GetCPUMemory();

		bool isInitialized() const { return bInitialized; }

		void InitAndUploadTextureToVideoMemory(ID3D12Device* device, ID3D12Resource* tmpResource, ID3D12GraphicsCommandList* commandList, std::vector<D3D12_SUBRESOURCE_DATA>& subresources);

		void CreateShaderResourceView(ID3D12Device* device, ShaderResourceType resourceType);

		auto& srvHeap() const { return mSrvHeap; }

	private:
		std::shared_ptr<void> mMemory = nullptr;
		ComPtr<ID3D12DescriptorHeap> mSrvHeap;
		bool bInitialized = false;
	};
}
