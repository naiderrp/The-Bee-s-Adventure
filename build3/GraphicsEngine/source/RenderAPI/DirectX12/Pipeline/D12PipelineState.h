#pragma once
#include <wrl.h>

#include "D12RootSignature.h"
#include "HLSLShader.h"

namespace GraphicsEngine {
	class GRAPHICS_API D12PipelineState : public Microsoft::WRL::ComPtr<ID3D12PipelineState>
	{
	public:
		D12PipelineState() = default;
		~D12PipelineState();

		inline ID3D12RootSignature* GetRootSignature() { return mRootSignature.Get(); }

		void Initialize(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);
		
		void InitializeAsTransparent(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);
		
		void InitializeAsSkybox(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);
		
		void InitializeForBulletDebugger(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);
		
		void InitializeForDrawingText(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);
		
		void InitializeForTextured(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);

		void InitializeForDrawing2D(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);
		
		void InitializeForDrawing2DTextured(ID3D12Device* pDevice, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath);

		void Release();

	private:
		D12RootSignature mRootSignature;
		HLSLShader mShaders[2]; // vertex and pixel shaders
	};
}