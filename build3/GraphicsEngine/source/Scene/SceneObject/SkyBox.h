#pragma once
#include "pch.h"
#include "VertexMesh.h"
namespace GraphicsEngine {
	class GRAPHICS_API SkyBox
	{
	public:
		void InitializeAsSphere(float radius);
	public:
		std::wstring texturePath = L"";
		VertexMesh model = {};
		float scaleFactor = 1.0f;
	public:
		D12Resource mCBPassData;
	};
}

