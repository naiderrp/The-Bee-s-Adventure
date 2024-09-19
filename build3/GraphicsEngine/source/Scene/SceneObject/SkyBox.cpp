#include "pch.h"
#include "SkyBox.h"

namespace GraphicsEngine {
	void SkyBox::InitializeAsSphere(float radius)
	{
        std::vector<Render::SkyBox::Vertex> vertices;
        std::vector<uint32_t> indices;

        const int stacks = 20;
        const int slices = 20;

        for (int i = 0; i <= stacks; ++i) {
            float phi = DirectX::XM_PI * i / stacks; // [0; PI]

            for (int j = 0; j <= slices; ++j) {
                float theta = 2.0f * DirectX::XM_PI * j / slices; // [0; 2PI]

                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * cosf(phi);
                float z = radius * sinf(phi) * sinf(theta);

                vertices.emplace_back(DirectX::XMFLOAT3(x, y, z));
            }
        }
        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
        model.Initialize<Render::SkyBox::Vertex>(getDefaultDevice(), vertices, indices);
	}
}
