#pragma once
#include <wrl.h>
#include "EngineMin.h"

namespace GraphicsEngine {
	class GRAPHICS_API D12CommandList : public Microsoft::WRL::ComPtr<ID3D12CommandList>
	{
	public:
		~D12CommandList();

		void Initialize(ID3D12Device* pDevice);
		void Release();

		inline ID3D12GraphicsCommandList* GFXCmd() { return static_cast<ID3D12GraphicsCommandList*>(Get()); }

		void ResetCommandList();

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	};
}
