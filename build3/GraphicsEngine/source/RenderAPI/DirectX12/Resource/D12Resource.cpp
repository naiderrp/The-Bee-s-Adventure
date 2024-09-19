#include "pch.h"
#include "D12Resource.h"

namespace GraphicsEngine {
	D12Resource::D12Resource(D12Resource&& other) noexcept
	{
		if (other.mMemory) { other->Unmap(0, 0); }

		if (other.Get()) {
			Swap(other);
		}

		if (Get() && other.mMemory) {
			void* mappedMemory = nullptr;
			HRESULT hr = Get()->Map(0, nullptr, &mappedMemory);
			if (SUCCEEDED(hr)) {
				mMemory.reset(mappedMemory, [](void* ptr) {});
			}
			other.mMemory = nullptr;
		}

	}
	D12Resource& D12Resource::operator=(D12Resource&& other) noexcept
	{
		if (other.mMemory) { other->Unmap(0, 0); }

		if (other.Get()) {
			Swap(other);
		}

		if (Get() && other.mMemory) {
			void* mappedMemory = nullptr;
			HRESULT hr = Get()->Map(0, nullptr, &mappedMemory);
			if (SUCCEEDED(hr)) {
				mMemory.reset(mappedMemory, [](void* ptr) {});
			}
			other.mMemory = nullptr;
		}

		return *this;
	}

	D12Resource::~D12Resource()
	{
		Release();
	}
	void D12Resource::Initialize(ID3D12Device* pDevice, UINT numBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState)
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = heapType;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = numBytes;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc = { 1,0 };
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		API_EVAL_HR(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, initialState, 0, IID_PPV_ARGS(GetAddressOf())), "Error creating a resource");

		bInitialized = true;
	}
	void D12Resource::Release()
	{
		if (!Get()) return;
		if (mMemory) {
			Get()->Unmap(0, nullptr);
			mMemory = nullptr;
		}
		Reset();
	}
	void D12Resource::InitializeAsDepthBuffer(ID3D12Device* pDevice, UINT width, UINT height)
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT; // CPU can't access 
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 0;
		resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
		resourceDesc.SampleDesc = { 1,0 };
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = resourceDesc.Format;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0.0f;

		API_EVAL_HR(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(GetAddressOf())), "Error creating a depth resource");
	}
	void* D12Resource::GetCPUMemory()
	{
		if (!Get()) return nullptr;
		if (!mMemory) {
			void* mappedMemory = nullptr;
			HRESULT hr = Get()->Map(0, nullptr, &mappedMemory);
			if (SUCCEEDED(hr)) {
				mMemory.reset(mappedMemory, [](void* ptr) {});
			}
		}
		return mMemory.get();
	}
	void D12Resource::InitAndUploadTextureToVideoMemory(ID3D12Device* device, ID3D12Resource* tmpResource, ID3D12GraphicsCommandList* commandList, std::vector<D3D12_SUBRESOURCE_DATA>& subresources)
	{
		D3D12_RESOURCE_DESC textureDesc = tmpResource->GetDesc();

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC uploadResourceDesc = {};
		uploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadResourceDesc.Width = GetRequiredIntermediateSize(tmpResource, 0, static_cast<UINT>(subresources.size()));
		uploadResourceDesc.Height = 1;
		uploadResourceDesc.DepthOrArraySize = 1;
		uploadResourceDesc.MipLevels = textureDesc.MipLevels;
		uploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadResourceDesc.SampleDesc.Count = textureDesc.SampleDesc.Count;
		uploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		API_EVAL_HR(
			device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&uploadResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(GetAddressOf())
			),
			"Failed to create texture upload heap"
		);

		UpdateSubresources(commandList, Get(), tmpResource, 0, 0, subresources.size(), subresources.data());
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		commandList->ResourceBarrier(1, &barrier);
	}
	void D12Resource::CreateShaderResourceView(ID3D12Device* device, ShaderResourceType resourceType)
	{
		if (!mSrvHeap.Get()) {
			D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
			srvHeapDesc.NumDescriptors = 1;
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			API_EVAL_HR(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mSrvHeap.GetAddressOf())), "Failed to create srv heap");
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

		switch (resourceType) {
		case ShaderResourceType::eTextureCube: {
			viewDesc.Format = Get()->GetDesc().Format;
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			viewDesc.TextureCube.MostDetailedMip = 0;
			viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			viewDesc.TextureCube.MipLevels = Get()->GetDesc().MipLevels;
			viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;
		}
		}
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
		device->CreateShaderResourceView(Get(), &viewDesc, srvHandle);
	}
}