#pragma once
#include <wrl.h>

namespace GraphicsEngine {
	class GRAPHICS_API D12RootSignature : public Microsoft::WRL::ComPtr<ID3D12RootSignature>
	{
	public:
		D12RootSignature() = default;
		~D12RootSignature();

		void Initialize(ID3D12Device* pDevice);
		
		void InitializeForSkybox(ID3D12Device* pDevice);
		
		void InitializeForText(ID3D12Device* pDevice);
		
		void InitializeForTextured(ID3D12Device* pDevice);
		
		void InitializeForDrawing2D(ID3D12Device* pDevice);
		
		void InitializeForDrawing2DTextured(ID3D12Device* pDevice);

		void Release();
	};
}