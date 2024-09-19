#pragma once
#include "RenderAPI/DirectX12/Renderer.h"
#include <Windows.h>
#include "pch.h"


namespace GraphicsEngine {
	class GRAPHICS_API Win32Application
	{
	public:
		bool Initialize();

		void Update();

		void OnCreate(HWND hwnd);

		void OnDestroy();

		inline bool isRunning() { return mIsRunning; }

	public: // subsystems
		Renderer renderer;
	public: // variables
		HWND mWindowHandle = nullptr;
		bool mIsRunning = false;

		UINT mWidth = 1280;
		UINT mHeight = 700;
	};
}
