#pragma once
#include <wrl.h>
#include "pch.h"

namespace GraphicsEngine {
	struct defInit {};

	class GRAPHICS_API D12Device : public Microsoft::WRL::ComPtr<ID3D12Device>
	{
	public:
		D12Device() = default;
		D12Device(const D12Device&) = default;
		D12Device(defInit);
		D12Device(IDXGIAdapter* pAdater);

		~D12Device() { if (Get()) Reset(); }

		void Init(IDXGIAdapter* pAdater);
		void Init(defInit);
	};

	ID3D12Device* getDefaultDevice();
}
