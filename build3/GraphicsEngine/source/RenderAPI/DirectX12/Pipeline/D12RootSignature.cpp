#include "pch.h"
#include "D12RootSignature.h"
#include "Utilities/d3dx12.h"

namespace GraphicsEngine {
	D12RootSignature::~D12RootSignature()
	{
		Release();
	}

	void D12RootSignature::Initialize(ID3D12Device* pDevice)
	{
		// b0 (t0) registerspace for a ConstantBuffer
		D3D12_ROOT_PARAMETER rootParameters[1];

		// pass data: MVP
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[0].Descriptor.ShaderRegister = 0;
		rootParameters[0].Descriptor.RegisterSpace = 0;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParameters;
		rsDesc.Desc_1_0.NumParameters = _countof(rootParameters);
		rsDesc.Desc_1_0.NumStaticSamplers = 0;
		rsDesc.Desc_1_0.pStaticSamplers = 0;
		rsDesc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> pSerializedRS = nullptr;
		ComPtr<ID3DBlob> pErrorBlob = nullptr;

		API_EVAL_HR(D3D12SerializeVersionedRootSignature(&rsDesc, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");

		if (pErrorBlob) {
#ifdef _DEBUG
			//PRINT_N("Rootsignature serialization error: " << (const char*)pErrorBlob->GetBufferPointer());
#endif
		}

		API_EVAL_HR(pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())), "Error creating the root signature");
	}

	void D12RootSignature::InitializeForSkybox(ID3D12Device* pDevice)
	{
		D3D12_ROOT_DESCRIPTOR rootDescriptor = {};
		rootDescriptor.RegisterSpace = 0;
		rootDescriptor.ShaderRegister = 0;

		D3D12_ROOT_PARAMETER rootParameters[2];

		// PassData: modelMatrix, viewProjectionMatrix, cameraPosition
		{
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[0].Descriptor = rootDescriptor;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		}
		// TextureCube CubeMap : register(t0)
		{
			D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1];
			descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorTableRanges[0].NumDescriptors = 1;
			descriptorTableRanges[0].BaseShaderRegister = 0;
			descriptorTableRanges[0].RegisterSpace = 0;
			descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
			descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
			descriptorTable.pDescriptorRanges = descriptorTableRanges;

			rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[1].DescriptorTable = descriptorTable;
			rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}
		// SamplerState linearWrapSamplerState : register(s0)
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParameters;
		rsDesc.Desc_1_0.NumParameters = _countof(rootParameters);
		rsDesc.Desc_1_0.NumStaticSamplers = 1;
		rsDesc.Desc_1_0.pStaticSamplers = &sampler;
		rsDesc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init(
			_countof(rootParameters),
			rootParameters,
			1,
			&sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		);

		ComPtr<ID3DBlob> pSerializedRS = nullptr;
		ComPtr<ID3DBlob> pErrorBlob = nullptr;

		//API_EVAL_HR(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");		
		API_EVAL_HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");

		if (pErrorBlob) {
#ifdef _DEBUG
			//PRINT_N("Rootsignature serialization error: " << (const char*)pErrorBlob->GetBufferPointer());
#endif
		}

		API_EVAL_HR(pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())), "Error creating the root signature");
	}

	void D12RootSignature::InitializeForText(ID3D12Device* pDevice)
	{
		D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1];
		descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorTableRanges[0].NumDescriptors = 1;
		descriptorTableRanges[0].BaseShaderRegister = 0;
		descriptorTableRanges[0].RegisterSpace = 0;
		descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
		descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
		descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

		D3D12_ROOT_DESCRIPTOR rootDescriptor = {};
		rootDescriptor.RegisterSpace = 0;
		rootDescriptor.ShaderRegister = 0;

		D3D12_ROOT_PARAMETER  rootParameters[2];
		
		// ConstantBuffer<PassData> gPassData : register(b0)
		{
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[0].Descriptor = rootDescriptor;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		}

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[1].DescriptorTable = descriptorTable; 
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(_countof(rootParameters),
			rootParameters, 
			1,
			&sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

		ComPtr<ID3DBlob> pSerializedRS = nullptr;
		ComPtr<ID3DBlob> pErrorBlob = nullptr;

		API_EVAL_HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");
		API_EVAL_HR(pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())), "Error creating the root signature");
	}

	void D12RootSignature::InitializeForTextured(ID3D12Device* pDevice)
	{
		D3D12_ROOT_DESCRIPTOR rootDescriptor = {};
		rootDescriptor.RegisterSpace = 0;
		rootDescriptor.ShaderRegister = 0;

		D3D12_ROOT_PARAMETER rootParameters[2];

		// ConstantBuffer<PassData> gPassData : register(b0);
		{
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[0].Descriptor = rootDescriptor;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		}
		// Texture2D t1 : register(t0);
		{
			D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1];
			descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorTableRanges[0].NumDescriptors = 1;
			descriptorTableRanges[0].BaseShaderRegister = 0;
			descriptorTableRanges[0].RegisterSpace = 0;
			descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
			descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
			descriptorTable.pDescriptorRanges = descriptorTableRanges;

			rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[1].DescriptorTable = descriptorTable;
			rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}

		// SamplerState s1 : register(s0);
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParameters;
		rsDesc.Desc_1_0.NumParameters = _countof(rootParameters);
		rsDesc.Desc_1_0.NumStaticSamplers = 1;
		rsDesc.Desc_1_0.pStaticSamplers = &sampler;
		rsDesc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init(
			_countof(rootParameters),
			rootParameters,
			1,
			&sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		);

		ComPtr<ID3DBlob> pSerializedRS = nullptr;
		ComPtr<ID3DBlob> pErrorBlob = nullptr;

		//API_EVAL_HR(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");		
		API_EVAL_HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");

		if (pErrorBlob) {
#ifdef _DEBUG
			//PRINT_N("Rootsignature serialization error: " << (const char*)pErrorBlob->GetBufferPointer());
#endif
		}

		API_EVAL_HR(pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())), "Error creating the root signature");
	}

	void D12RootSignature::InitializeForDrawing2D(ID3D12Device* pDevice)
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init(
			0,
			nullptr,
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		);

		ComPtr<ID3DBlob> pSerializedRS = nullptr;
		ComPtr<ID3DBlob> pErrorBlob = nullptr;

		API_EVAL_HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");

		if (pErrorBlob) {
#ifdef _DEBUG
			//PRINT_N("Rootsignature serialization error: " << (const char*)pErrorBlob->GetBufferPointer());
#endif
		}

		API_EVAL_HR(pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())), "Error creating the root signature");
	}

	void D12RootSignature::InitializeForDrawing2DTextured(ID3D12Device* pDevice)
	{
		D3D12_ROOT_DESCRIPTOR rootDescriptor = {};
		rootDescriptor.RegisterSpace = 0;
		rootDescriptor.ShaderRegister = 0;

		D3D12_ROOT_PARAMETER rootParameters[1];

		// Texture2D t1 : register(t0);
		{
			D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1];
			descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorTableRanges[0].NumDescriptors = 1;
			descriptorTableRanges[0].BaseShaderRegister = 0;
			descriptorTableRanges[0].RegisterSpace = 0;
			descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
			descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
			descriptorTable.pDescriptorRanges = descriptorTableRanges;

			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[0].DescriptorTable = descriptorTable;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}

		// SamplerState s1 : register(s0);
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;  // Используем режим BORDER для U
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;  // Используем режим BORDER для V
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;  // Используем режим BORDER для W
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;  // Граница черного цвета
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParameters;
		rsDesc.Desc_1_0.NumParameters = _countof(rootParameters);
		rsDesc.Desc_1_0.NumStaticSamplers = 1;
		rsDesc.Desc_1_0.pStaticSamplers = &sampler;
		rsDesc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init(
			_countof(rootParameters),
			rootParameters,
			1,
			&sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		);

		ComPtr<ID3DBlob> pSerializedRS = nullptr;
		ComPtr<ID3DBlob> pErrorBlob = nullptr;

		API_EVAL_HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");

		if (pErrorBlob) {
#ifdef _DEBUG
			//PRINT_N("Rootsignature serialization error: " << (const char*)pErrorBlob->GetBufferPointer());
#endif
		}

		API_EVAL_HR(pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())), "Error creating the root signature");
	}

	void D12RootSignature::Release()
	{
		if (Get()) Reset();
	}
}