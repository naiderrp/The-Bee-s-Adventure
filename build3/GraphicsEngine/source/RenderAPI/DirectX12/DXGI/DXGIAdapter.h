#pragma once
#include <wrl.h>

namespace GraphicsEngine {
	class DXGIAdapter : public Microsoft::WRL::ComPtr<IDXGIAdapter>
	{
	public:
		DXGIAdapter() = default;
		DXGIAdapter(IDXGIAdapter* pAdater);
	};
}
