#pragma once
#include <wrl.h>

namespace GraphicsEngine {
	class DXGIDebug : public Microsoft::WRL::ComPtr<IDXGIDebug>
	{
	public:
		static DXGIDebug& Get() { return mInstance; }
		void Enable();
		void GetLiveObjects();
	private:
		void GetInterface();
	private:
		static DXGIDebug mInstance;
	};
}

