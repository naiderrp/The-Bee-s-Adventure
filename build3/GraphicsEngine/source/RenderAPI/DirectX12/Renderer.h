#pragma once
#include "EngineMin.h"
#include <Windows.h>
#include "Base/D12Device.h"
#include "Commands/D12CommandQueue.h"
#include "Commands/D12CommandList.h"
#include "DXGI/DXGISwapChain.h"
#include "Resource/D12Resource.h"
#include "RendererDataTypes.h"
#include "Pipeline/D12PipelineState.h"
#include "Utilities/Utilities.h"
#include "Descriptors/D12DescriptorHeap.h"
#include "MemoryManagement/D12BufferUploader.h"
#include <vector>

#include <Utilities/DDSTextureLoader12.h>

#include "Scene/SceneObject/Object3D.h"
#include "Scene/SceneObject/ScreenText.h"
#include "Scene/GraphicsScene.h"
#include "Scene/Camera.h"

namespace GraphicsEngine {
	class GRAPHICS_API Renderer
	{
	public:
		~Renderer();

		void setScene(GraphicsScene& scene);

		void Initialize(HWND hwnd, UINT width, UINT height);
		void Release();

		void UpdateDraw();

		GraphicsScene* scene();

	private:
		void LoadSkyBoxTexture();
	
		void LoadTexture(const wchar_t* path, Microsoft::WRL::ComPtr<ID3D12Resource>& textureBuffer);

		void RenderText(ScreenText& text, const DirectX::XMFLOAT2& pos = { 0.0f, 0.0f });
	
	public:
		D12Device mDevice;

	private:
		GraphicsScene* mScene = nullptr;

		D12CommandQueue mCommandQueue;
		D12CommandList mCommandList;

		DXGISwapChain mSwapChain;

		D12Resource mDepthBuffer;

		D12DescriptorHeap mDepthHeap;

		D12PipelineState mBasePipeline;

		D12PipelineState mDrawing2DPipeline;
		D12PipelineState mDrawing2DTexturedPipeline;

		D12PipelineState mTexturedPipeline;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource> mTrampolineTextureBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> mStartFinishTextureBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> mButtonTextureBuffer;

		D12PipelineState mSkyBoxPipeline;
		D12Resource mSkyBoxTexture;

		D3D12_VIEWPORT mViewport;
		D3D12_RECT mSRRect;

	private:
		UINT mWidth = 0;
		UINT mHeight = 0;
	};

}
