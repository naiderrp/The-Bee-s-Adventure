#include "pch.h"
#include "Win32Application.h"
#include <windowsx.h>

namespace GraphicsEngine {
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		switch (msg) {
		case WM_NCCREATE: {
			LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lparam);
			Win32Application* ptr = reinterpret_cast<Win32Application*>(param->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
			
			break;
		}
		case WM_CREATE: {
			Win32Application* ptr = reinterpret_cast<Win32Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			ptr->OnCreate(hwnd);
			break;
		}
		case WM_DESTROY: {
			Win32Application* ptr = reinterpret_cast<Win32Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			ptr->OnDestroy();
			PostQuitMessage(0);
		}
		case WM_MOUSEWHEEL: {
			Win32Application* ptr = reinterpret_cast<Win32Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			ptr->renderer.scene()->camera->mouseWheelDelta() += GET_WHEEL_DELTA_WPARAM(wparam) / (float)WHEEL_DELTA;
			break;
		}
		case WM_LBUTTONDOWN: {
			int xPos = GET_X_LPARAM(lparam);
			int yPos = GET_Y_LPARAM(lparam);

			Win32Application* ptr = reinterpret_cast<Win32Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			
			if (!ptr->renderer.scene()->button) break;

			auto bottomLeft = ptr->renderer.scene()->button->bottomLeft;
			auto topLeft = ptr->renderer.scene()->button->topLeft;
			auto topRight = ptr->renderer.scene()->button->topRight;
			auto bottomRight = ptr->renderer.scene()->button->bottomRight;

			RECT rect;
			GetClientRect(hwnd, &rect);
			int screenWidth = rect.right - rect.left;
			int screenHeight = rect.bottom - rect.top;

			float xNDC = (2.0f * xPos / screenWidth) - 1.0f;
			float yNDC = 1.0f - (2.0f * yPos / screenHeight);

			if (xNDC >= bottomLeft.x && xNDC <= bottomRight.x && yNDC >= bottomLeft.y && yNDC <= topLeft.y)
				ptr->renderer.scene()->button->Click();
				
			break;
		}
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	bool Win32Application::Initialize()
	{
		mWidth = GetSystemMetrics(SM_CXSCREEN);
		mHeight = GetSystemMetrics(SM_CYSCREEN);

		WNDCLASS wndClass = {};
		wndClass.lpszClassName = L"BaseWindowClass";
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wndClass.lpfnWndProc = WndProc;
		
		RegisterClass(&wndClass);
		mWindowHandle = CreateWindow(L"BaseWindowClass", L"Game", WS_OVERLAPPEDWINDOW, 0, 0, mWidth, mHeight, 0, 0, 0, this);

		if (!mWindowHandle) return false;

		ShowWindow(mWindowHandle, SW_SHOW);
		UpdateWindow(mWindowHandle);

		mIsRunning = true;
		return mIsRunning;
	}
	void Win32Application::Update()
	{
		MSG message;
		(PeekMessage(&message, 0, 0, 0, PM_REMOVE));
		TranslateMessage(&message);
		DispatchMessage(&message);

		renderer.UpdateDraw();
	}

	void Win32Application::OnCreate(HWND hwnd)
	{
		std::cout << "Initializing graphics context\n";
		renderer.Initialize(hwnd, mWidth, mHeight);
	}
	void Win32Application::OnDestroy()
	{
		std::cout << "Shutting down the application\n";
		mIsRunning = false;
	}
}
