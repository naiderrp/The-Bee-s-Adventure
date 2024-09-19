#pragma once
#include "pch.h"
#include "VertexMesh.h"

namespace GraphicsEngine {
    struct GRAPHICS_API TransformComponent {
        DirectX::XMFLOAT3 translation{0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 scale{ 1.f, 1.f, 1.f };
        DirectX::XMFLOAT3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        DirectX::XMMATRIX mat4() {
            const float c3 = cos(rotation.z);
            const float s3 = sin(rotation.z);
            const float c2 = cos(rotation.x);
            const float s2 = sin(rotation.x);
            const float c1 = cos(rotation.y);
            const float s1 = sin(rotation.y);
            return DirectX::XMMATRIX{
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0.0f,
                },
                {translation.x, translation.y, translation.z, 1.0f} };
        }
    };

    class GRAPHICS_API Object3D
    {
    public:
        void InitializeAsBox(float width, float height, float depth, const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 1.0f, 1.0f });
        
        void InitializeAsSphere(D12Device& device, float radius, const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 1.0f, 1.0f });
        
        void InitializeAsArrow(const DirectX::XMFLOAT3 &direction, const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 1.0f, 1.0f });

        void InitializeAsTexturedRectangle(float width, float height, float depth, const char* texturePath = "");

        void InitializeAsObjModel(const char* objPath);

    public:
        TransformComponent transform = {};
        VertexMesh model = {};
        DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 1.0f, 1.0f };
        std::string texturePath = "";
    public:
        D12Resource mCBPassData;
    };

    struct GRAPHICS_API Line3D {
        DirectX::XMFLOAT3 from;
        DirectX::XMFLOAT3 to;
        DirectX::XMFLOAT4 color = { 1.0, 1.0, 1.0, 1.0 };
    };
}

