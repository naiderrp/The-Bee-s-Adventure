#include "pch.h"
#include "DXGIAdapter.h"

GraphicsEngine::DXGIAdapter::DXGIAdapter(IDXGIAdapter* pAdater) : Microsoft::WRL::ComPtr<IDXGIAdapter>(pAdater)
{
}
