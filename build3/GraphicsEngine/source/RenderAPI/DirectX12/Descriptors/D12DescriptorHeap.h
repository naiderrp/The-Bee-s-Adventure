#pragma once
#include <wrl.h>

namespace GraphicsEngine {
	class GRAPHICS_API D12DescriptorHeap : public Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> {
	public:
		~D12DescriptorHeap();

		void InitializeAsDepthHeap(ID3D12Device* pDevice, UINT width, UINT height);
		void Release();
	};
}