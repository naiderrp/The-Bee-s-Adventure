#include "pch.h"
#include "HLSLShader.h"

namespace GraphicsEngine {
	HLSLShader::~HLSLShader()
	{
		Release();
	}

	void HLSLShader::Initialize(const LPCWSTR filename, const ShaderType shaderType, LPCSTR entryPoint)
	{
		if (!exist(filename)) {
#ifdef _DEBUG
			PRINT_W_N("Error: " << filename << " does not exist");
#endif
			return;
		}

		// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/d3dcompile-constants
		UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ALL_RESOURCES_BOUND;

		const char* target = "";

		switch (shaderType) {
		case ShaderType::eVertex: {
			target = "vs_5_1";
			break;
		}
		case ShaderType::ePixel: {
			target = "ps_5_1";
			break;
		}

		default: {
#ifdef _DEBUG
			//PRINT_N("Error unspported shader compilation target");
#endif
			return;
		}
		}

		ID3DBlob* errorMsg = nullptr;
		HRESULT result = D3DCompileFromFile(filename, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, flags, 0, &mByteCode, &errorMsg);

		if (result != S_OK) {
#ifdef _DEBUG
			//PRINT_N("Shader loading error was: " << result);
#endif
		}

		if (errorMsg) {
#ifdef _DEBUG
			//PRINT_N("Shader compliation error: " << (const char*)errorMsg->GetBufferPointer());
#endif
			return;
		}

#ifdef _DEBUG
		//PRINT_W_N("Loaded the shader: " << filename);
#endif
	}

	void HLSLShader::Release()
	{
		if (mByteCode)
			if (mByteCode->GetBufferPointer()) {
				mByteCode->Release();
				mByteCode = nullptr;
			}
	}
}
