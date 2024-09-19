#pragma once
#include <wrl.h>
#include "pch.h"

namespace GraphicsEngine {
	class GRAPHICS_API DXGISwapChain : public Microsoft::WRL::ComPtr<IDXGISwapChain>
	{
	public:
		~DXGISwapChain();

		void Initialize(ID3D12Device* pDevice, IDXGIFactory* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, UINT width, UINT height);
		void Release();

		void CreateRenderTargets();
		void ResetRenderTargets();

		void Present();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle();
		inline ID3D12Resource* GetCurrentRenderTarget() { return mRenderTargets[mCurrentBuffer].Get(); }
		inline UINT GetCurrentFrameIndex() const { return mCurrentBuffer; }
	private:
		ID3D12Device* mDevice = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTargets[MAX_SWAPCHAIN_BUFFERS];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRTVHeap;

		UINT mCurrentBuffer = 0;

		UINT mWidth = 0;
		UINT mHeight = 0;

		UINT mAllocationSize = 0;
		UINT mHeapIncrement = 0;
	};
}
