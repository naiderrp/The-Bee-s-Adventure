#pragma once
#include "pch.h"
#include <wrl.h>
#include <fstream>

namespace GraphicsEngine {
	class GRAPHICS_API HLSLShader
	{
	public:
		enum ShaderType {
			eVertex,
			ePixel
		};

	public:
		~HLSLShader();

		void Initialize(const LPCWSTR filename, const ShaderType shaderType, LPCSTR entryPoint = "main");
		void Release();

		inline ID3DBlob* GetByteBlob() { return mByteCode.Get(); }

	private:
		inline bool exist(const LPCWSTR path) { std::ifstream fin; fin.open(path); return bool(fin); }

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> mByteCode;
	};
}
