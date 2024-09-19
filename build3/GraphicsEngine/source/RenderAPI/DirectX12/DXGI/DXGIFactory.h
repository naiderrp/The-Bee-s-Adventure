#pragma once
#include <wrl.h>
#include "DXGIAdapter.h"

namespace GraphicsEngine {
	class DXGIFactory : public Microsoft::WRL::ComPtr<IDXGIFactory>
	{
	public:
		DXGIFactory();

		DXGIAdapter GetAdapter();
	};
}

