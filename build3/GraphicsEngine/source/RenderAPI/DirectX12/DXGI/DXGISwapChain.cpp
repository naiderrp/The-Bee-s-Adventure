#include "pch.h"
#include "DXGISwapChain.h"

namespace GraphicsEngine {

	DXGISwapChain::~DXGISwapChain()
	{
		Release();
	}

	void DXGISwapChain::Initialize(ID3D12Device* pDevice, IDXGIFactory* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, UINT width, UINT height)
	{
		mDevice = pDevice;

		mWidth = width;
		mHeight = height;

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = MAX_SWAPCHAIN_BUFFERS;

		API_EVAL_HR(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRTVHeap.GetAddressOf())), "Error creating the RTV heap");
		mRTVHeap->SetName(L"Swapchain buffers RTV heap");
		mHeapIncrement = mDevice->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

		DXGI_SWAP_CHAIN_DESC desc = {}; 
		desc.BufferDesc.Width = mWidth;
		desc.BufferDesc.Height = mHeight;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc = { 1, 0 };
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = MAX_SWAPCHAIN_BUFFERS;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED; // DXGI_SCALING_NONE
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Windowed = true;
		desc.OutputWindow = hwnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//API_EVAL_HR(pFactory->CreateSwapChainForHwnd(pCommandQueue, hwnd, &desc, nullptr, nullptr, GetAddressOf()), "Error creating swapchain");
		API_EVAL_HR(pFactory->CreateSwapChain(pCommandQueue, &desc, GetAddressOf()), "Error creating swapchain");
		
		CreateRenderTargets();
	}

	void DXGISwapChain::Release()
	{
		if (!Get()) return;
		Reset();
		ResetRenderTargets();
		mRTVHeap.Reset();
	}

	void DXGISwapChain::ResetRenderTargets()
	{
		for (auto& renderTarget : mRenderTargets) renderTarget.Reset();
	}

	void DXGISwapChain::Present()
	{
		//Get()->Present(0, 0); // DXGI_PRESENT_ALLOW_TEARING
		Get()->Present(1, 0); // FIFO present mode (60 FPS)
		mCurrentBuffer = (mCurrentBuffer + 1) % _countof(mRenderTargets);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DXGISwapChain::GetCurrentRTVHandle()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += (size_t)mHeapIncrement * mCurrentBuffer;
		return cpuHandle;
	}

	void DXGISwapChain::CreateRenderTargets()
	{
		if (!Get()) {
#ifdef _DEBUG
			//PRINT_N("Error: Tried to create buffers for uninitialized swapchain");
#endif
			return;
		}
		for (int i = 0; i < _countof(mRenderTargets); ++i) {
			mRenderTargets[i].Reset();

			API_EVAL_HR(Get()->GetBuffer(i, IID_PPV_ARGS(mRenderTargets[i].GetAddressOf())), "Error creating buffer for swapchain index: " << i);

			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (size_t)mHeapIncrement * i;

			mDevice->CreateRenderTargetView(mRenderTargets[i].Get(), 0, cpuHandle);
		}
	}
}