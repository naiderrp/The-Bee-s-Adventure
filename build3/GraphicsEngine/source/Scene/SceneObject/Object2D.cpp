#include "pch.h"
#include "Object2D.h"

namespace GraphicsEngine {
	void Object2D::InitializeAsRectangle(screen_space_coordinate_t bottomLeft, screen_space_coordinate_t topLeft, screen_space_coordinate_t topRight, screen_space_coordinate_t bottomRight, const DirectX::XMFLOAT4& color)
	{
        std::vector<Render::Drawing2D::Vertex> vertices = {
            { {bottomLeft}, color },
            { {topLeft}, color },
            { {topRight}, color },
            { {bottomRight}, color}
        };

        std::vector<uint32_t> indices = {
            0, 1, 2, 0, 2, 3
        };

        model.Initialize<Render::Drawing2D::Vertex>(getDefaultDevice(), vertices, indices);
	}
    void Object2D::InitializeAsTexturedRectangle(screen_space_coordinate_t bottomLeft, screen_space_coordinate_t bottomRight, screen_space_coordinate_t topRight, screen_space_coordinate_t topLeft)
    {
        std::vector<Render::Drawing2D::Textured::Vertex> vertices = {
            { {bottomLeft}, {0, 0} },
            { {topLeft}, {1, 0} },
            { {topRight}, {1, 1} },
            { {bottomRight}, {0, 1}}
        };

        std::vector<uint32_t> indices = {
            0, 1, 2, 0, 2, 3
        };

        model.Initialize<Render::Drawing2D::Textured::Vertex>(getDefaultDevice(), vertices, indices);
    }
}