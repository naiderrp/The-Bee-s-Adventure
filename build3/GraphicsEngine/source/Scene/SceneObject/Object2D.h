#pragma once
#include "VertexMesh.h"

namespace GraphicsEngine {
	struct GRAPHICS_API Object2D
	{
		using screen_space_coordinate_t = const DirectX::XMFLOAT2&;
	
		void InitializeAsRectangle(screen_space_coordinate_t bottomLeft, screen_space_coordinate_t topLeft, screen_space_coordinate_t topRight, screen_space_coordinate_t bottomRight, const DirectX::XMFLOAT4& color);

		void InitializeAsTexturedRectangle(screen_space_coordinate_t bottomLeft, screen_space_coordinate_t bottomRight, screen_space_coordinate_t topRight, screen_space_coordinate_t topLeft);

		VertexMesh model = {};
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}
