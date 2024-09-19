#pragma once
#ifdef ENGINE_EXPORTS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif // ENGINE_EXPORTS

#define MAX_SWAPCHAIN_BUFFERS 2 // max frames in flight