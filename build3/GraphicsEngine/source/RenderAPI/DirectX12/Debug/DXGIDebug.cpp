#include "pch.h"
#include "DXGIDebug.h"

namespace GraphicsEngine {
	DXGIDebug DXGIDebug::mInstance;

	void DXGIDebug::Enable()
	{
		GetInterface();
	}
	void DXGIDebug::GetLiveObjects()
	{
		GetInterface();
		Get()->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}
	void DXGIDebug::GetInterface()
	{
		if (!Get()) {
			API_EVAL_HR(DXGIGetDebugInterface1(0, IID_PPV_ARGS(GetAddressOf())), "Error while getting the debug interface");
		}
	}
}