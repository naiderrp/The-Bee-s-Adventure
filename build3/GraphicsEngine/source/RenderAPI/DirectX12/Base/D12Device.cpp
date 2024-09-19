#include "pch.h"
#include "D12Device.h"
#include "RenderAPI/DirectX12/DXGI/DXGIAdapter.h"
#include "RenderAPI/DirectX12/DXGI/DXGIFactory.h"

namespace GraphicsEngine {
	D12Device::D12Device(defInit)
	{
		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();
#ifdef _DEBUG
		//DXGI_ADAPTER_DESC desc;
		//adapter->GetDesc(&desc);
		//PRINT_W_N("Default device: " << desc.Description);
#endif
		Init(adapter.Get());
		Get()->SetName(L"Default virtual device");
	}

	D12Device::D12Device(IDXGIAdapter* pAdater)
	{
		Init(pAdater);
	}

	void D12Device::Init(IDXGIAdapter* pAdater)
	{
		API_EVAL_HR(D3D12CreateDevice(pAdater, D3D_FEATURE_LEVEL_1_0_CORE, IID_PPV_ARGS(GetAddressOf())), "Error creating D12 device");
	}
	void D12Device::Init(defInit)
	{
		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();
#ifdef _DEBUG
		//DXGI_ADAPTER_DESC desc;
		//adapter->GetDesc(&desc);
		//PRINT_W_N("Default device: " << desc.Description);
#endif
		Init(adapter.Get());
		Get()->SetName(L"Default virtual device");
	}

	ID3D12Device* getDefaultDevice() {
		static D12Device device;
		if (!device.Get()) device.Init(defInit{});
		return device.Get();
	}
}