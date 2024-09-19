#pragma once
#include "pch.h"

#include "RenderAPI/DirectX12/RendererDataTypes.h"
#include "RenderAPI/DirectX12/Base/D12Device.h"
#include "RenderAPI/DirectX12/Resource/D12Resource.h"
#include "RenderAPI/DirectX12/MemoryManagement/D12BufferUploader.h"
#include "RenderAPI/DirectX12/Commands/D12CommandList.h"
#include "RenderAPI/DirectX12/Commands/D12CommandQueue.h"

#include <vector>

namespace GraphicsEngine {
	class VertexMesh
	{
	public:
		template<typename Vertex_Type>
		void Initialize(ID3D12Device* device, const std::vector<Vertex_Type>& vertices) {
			D12CommandList commandList;
			D12CommandQueue commandQueue;

			commandQueue.Initialize(device);
			commandQueue->SetName(L"Temporary Command Queue for vertex buffer upload");

			commandList.Initialize(device);
			commandList->SetName(L"Temporary Command List for vertex buffer upload");

			mVerticesCount = vertices.size();
			size_t vertexBufferSize = sizeof(vertices[0]) * mVerticesCount;
			mVertexBuffer.Initialize(device, vertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);

			D12BufferUploader bufferUploader;

			bufferUploader.Initialize(device, vertexBufferSize);
			bufferUploader.Upload((D12Resource*)mVertexBuffer.GetAddressOf(), (void*)vertices.data(), vertexBufferSize,
				(D12CommandList*)commandList.GetAddressOf(), (D12CommandQueue*)commandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			mVertexBufferView.BufferLocation = mVertexBuffer.Get()->GetGPUVirtualAddress();
			mVertexBufferView.StrideInBytes = sizeof(vertices[0]);
			mVertexBufferView.SizeInBytes = vertexBufferSize;

			bufferUploader.Release();
			commandList.Release();
			commandQueue.Release();
		}

		template<typename Vertex_Type>
		void Initialize(ID3D12Device* device, const std::vector<Vertex_Type>& vertices, const std::vector<uint32_t>& indices) {
			Initialize<Vertex_Type>(device, vertices);

			D12CommandList commandList;
			D12CommandQueue commandQueue;

			commandQueue.Initialize(device);
			commandQueue->SetName(L"Temporary Command Queue for index buffer upload");

			commandList.Initialize(device);
			commandList->SetName(L"Temporary Command List for index buffer upload");

			mIndicesCount = indices.size();
			size_t indexBufferSize = sizeof(indices[0]) * mIndicesCount;
			mIndexBuffer.Initialize(device, indexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mIndexBuffer->SetName(L"Index buffer");

			D12BufferUploader bufferUploader;
			bufferUploader.Initialize(device, indexBufferSize);
			bufferUploader.Upload((D12Resource*)mIndexBuffer.GetAddressOf(), (void*)indices.data(), indexBufferSize,
				(D12CommandList*)commandList.GetAddressOf(), (D12CommandQueue*)commandQueue.GetAddressOf());

			mIndexBufferView.BufferLocation = mIndexBuffer.Get()->GetGPUVirtualAddress();
			mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			mIndexBufferView.SizeInBytes = indexBufferSize;

			bufferUploader.Release();
			commandList.Release();
			commandQueue.Release();
		}

		auto verticesCount() const { return mVerticesCount; }
		auto indicesCount() const { return mIndicesCount; }

		auto& vertexBuffer() const { return mVertexBuffer; }
		auto& indexBuffer() const { return mIndexBuffer; }

		auto vertexBufferView() const { return mVertexBufferView; }
		auto indexBufferView() const { return mIndexBufferView; }

	private:
		size_t mVerticesCount = 0;
		size_t mIndicesCount = 0;

		D12Resource mVertexBuffer;
		D12Resource mIndexBuffer;

		D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
		D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	};
}

