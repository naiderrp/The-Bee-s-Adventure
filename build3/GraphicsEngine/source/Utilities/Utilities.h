#pragma once
#include "pch.h"
#include "RenderAPI/DirectX12/Base/D12Device.h"

namespace GraphicsEngine {
	namespace Utils {
		uint32_t CalculateConstantbufferAlignment(uint32_t allocation);

		int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow);

		int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);

		WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);

		DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
	}
}