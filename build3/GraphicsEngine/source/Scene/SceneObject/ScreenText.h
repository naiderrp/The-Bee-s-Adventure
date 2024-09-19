#pragma once
#include <pch.h>

#include "RenderAPI/DirectX12/Pipeline/D12PipelineState.h"
#include "RenderAPI/DirectX12/Resource/D12Resource.h"
#include "RenderAPI/DirectX12/RendererDataTypes.h"

#include "Object3D.h"

#include <string>

namespace GraphicsEngine {
	class GRAPHICS_API ScreenText
	{
	public:
		void InitializeGraphicsContext(D12Device& device, D12CommandList& commandList);
		
		void InitializeGraphicsContext(D12Device& device, D12CommandList& commandList, D12CommandQueue &commandQueue);

		void InitializeFont(LPCWSTR fontFile, int windowWidth, int windowHeight);
	
		int MaxNumTextCharacters() const { return mMaxNumTextCharacters; }
		Render::Text::Font& Font() { return mFont; }

		Render::Text::GraphicsContext GetGraphicsContext();
		
		void DrawInScreenSpace(bool on = true) {
			mScreenSpace = on;
		}

		bool InScreenSpace() const {
			return mScreenSpace;
		}

		bool IsInitialized() const { return mWidth != 0; }

	private:
		Render::Text::Font LoadFontFromFile(LPCWSTR filename, int windowWidth, int windowHeight);

	public:
		std::wstring text = L"";
		LPCWSTR font = L"fonts/ComicSansMS_Extended.fnt";

		TransformComponent textTransformComponent;
		
		DirectX::XMFLOAT2 scale = { 3.5, 3.5 };
		DirectX::XMFLOAT2 pos = { 0.0, 0.0 };
		DirectX::XMFLOAT2 padding = { 0.0f, 0.0f };
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	private:
		Render::Text::Font mFont;
		D12Resource mTextPassData;
		D12PipelineState mTextPipeline;
		const int mMaxNumTextCharacters = 1024;

		int mWidth = 0;
		int mHeight = 0;

		Microsoft::WRL::ComPtr<ID3D12Resource> mTextVertexBuffer[MAX_SWAPCHAIN_BUFFERS];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mTextDescriptorHeap;
		D3D12_VERTEX_BUFFER_VIEW mTextVertexBufferView[MAX_SWAPCHAIN_BUFFERS];
		UINT8* mTextVBGPUAddress[MAX_SWAPCHAIN_BUFFERS];

		bool mScreenSpace = false;
	};
}
