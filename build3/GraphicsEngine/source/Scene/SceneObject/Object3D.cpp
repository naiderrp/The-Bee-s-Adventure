#include "pch.h"
#include "Object3D.h"

#include "Utilities/Utilities.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "Utilities/tiny_obj_loader.h"

#include "RenderAPI/DirectX12/Base/D12Device.h"

#include <unordered_map>

namespace GraphicsEngine {
    void Object3D::InitializeAsBox(float width, float height, float depth, const DirectX::XMFLOAT4& color)
    {
        float w = static_cast<float>(width) / 2.0f;
        float h = static_cast<float>(height) / 2.0f;
        float d = static_cast<float>(depth) / 2.0f;

        std::vector<Render::Vertex> vertices =
        {
            // Front face
            { DirectX::XMFLOAT3(-w, -h, -d), color },
            { DirectX::XMFLOAT3(-w,  h, -d), color },
            { DirectX::XMFLOAT3(w,  h, -d), color },
            { DirectX::XMFLOAT3(w, -h, -d), color },

            // Back face
            { DirectX::XMFLOAT3(-w, -h, d), color },
            { DirectX::XMFLOAT3(-w,  h, d), color },
            { DirectX::XMFLOAT3(w,  h, d), color },
            { DirectX::XMFLOAT3(w, -h, d), color },

            // Left face
            { DirectX::XMFLOAT3(-w, -h, d), color },
            { DirectX::XMFLOAT3(-w,  h, d), color },
            { DirectX::XMFLOAT3(-w,  h, -d), color },
            { DirectX::XMFLOAT3(-w, -h, -d), color },

            // Right face
            { DirectX::XMFLOAT3(w, -h, d), color },
            { DirectX::XMFLOAT3(w,  h, d), color },
            { DirectX::XMFLOAT3(w,  h, -d), color },
            { DirectX::XMFLOAT3(w, -h, -d), color },

            // Top face
            { DirectX::XMFLOAT3(-w, h, d), color },
            { DirectX::XMFLOAT3(-w, h, -d), color },
            { DirectX::XMFLOAT3(w, h, -d), color },
            { DirectX::XMFLOAT3(w, h, d), color },

            // Bottom face
            { DirectX::XMFLOAT3(-w, -h, d), color },
            { DirectX::XMFLOAT3(-w, -h, -d), color },
            { DirectX::XMFLOAT3(w, -h, -d), color },
            { DirectX::XMFLOAT3(w, -h, d), color },
        };

        std::vector<uint32_t> indices =
        {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 6, 5, 4, 7, 6,
            // Left face
            8, 9, 10, 8, 10, 11,
            // Right face
            12, 14, 13, 12, 15, 14,
            // Top face
            16, 17, 18, 16, 18, 19,
            // Bottom face
            20, 22, 21, 20, 23, 22
        };

        model.Initialize<Render::Vertex>(getDefaultDevice(), vertices, indices);
    }

    void Object3D::InitializeAsSphere(D12Device& device, float radius, const DirectX::XMFLOAT4& color)
    {
        std::vector<Render::Vertex> vertices;
        std::vector<uint32_t> indices;

        const int stacks = 20;
        const int slices = 20;

        for (int i = 0; i <= stacks; ++i) {
            float phi = DirectX::XM_PI * i / stacks; // от 0 до PI

            for (int j = 0; j <= slices; ++j) {
                float theta = 2.0f * DirectX::XM_PI * j / slices; // от 0 до 2PI

                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * cosf(phi);
                float z = radius * sinf(phi) * sinf(theta);

                vertices.emplace_back(DirectX::XMFLOAT3(x, y, z), color);
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
        model.Initialize<Render::Vertex>(device.Get(), vertices, indices);
    }

    void Object3D::InitializeAsObjModel(const char* objPath)
    {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "";

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(objPath, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }
#ifdef _DEBUG
        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }
#endif
        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        std::vector<Render::Vertex> vertices;
        std::vector<uint32_t> indices;

        std::unordered_map<Render::Vertex, uint32_t> uniqueVertices = {};

        for (size_t s = 0; s < shapes.size(); s++) {
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                int material_id = shapes[s].mesh.material_ids[f];
                tinyobj::material_t material = materials[material_id];

                // Kd (Diffuse Color)
                DirectX::XMFLOAT4 diffuseColor(
                    material.diffuse[0], // R
                    material.diffuse[1], // G
                    material.diffuse[2], // B
                    (material.name == "Wings") ? 0.3f : 1.0f  // A
                );

                for (size_t v = 0; v < fv; v++) {
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    // Position
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                    DirectX::XMFLOAT3 position(vx, vy, vz);

                    Render::Vertex vertex = {};
                    vertex.position = position;
                    vertex.color = diffuseColor;

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }
                    indices.push_back(uniqueVertices[vertex]);
                }

                index_offset += fv;
            }
        }
        model.Initialize<Render::Vertex>(getDefaultDevice(), vertices, indices);
    }
    
    void Object3D::InitializeAsArrow(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT4& color)
    {
        std::vector<Render::Vertex> vertices;
        std::vector<uint32_t> indices;

        // Arrow parameters
        float shaftLength = 0.8f;
        float shaftWidth = 0.15f;
        float headLength = 0.5f;
        float headWidth = 0.45f;

        // Shaft vertices (in XZ plane, pointing along the +X axis)
        vertices.emplace_back(DirectX::XMFLOAT3(0.0f, 0.0f, -shaftWidth * 0.5f), color);  // Bottom left of the shaft
        vertices.emplace_back(DirectX::XMFLOAT3(shaftLength, 0.0f, -shaftWidth * 0.5f), color);  // Bottom right of the shaft
        vertices.emplace_back(DirectX::XMFLOAT3(shaftLength, 0.0f, shaftWidth * 0.5f), color);  // Top right of the shaft
        vertices.emplace_back(DirectX::XMFLOAT3(0.0f, 0.0f, shaftWidth * 0.5f), color);   // Top left of the shaft

        // Arrowhead vertices
        vertices.emplace_back(DirectX::XMFLOAT3(shaftLength, 0.0f, -headWidth * 0.5f), color);   // Bottom left of the arrowhead
        vertices.emplace_back(DirectX::XMFLOAT3(shaftLength + headLength, 0.0f, 0.0f), color);  // Tip of the arrowhead
        vertices.emplace_back(DirectX::XMFLOAT3(shaftLength, 0.0f, headWidth * 0.5f), color);   // Top left of the arrowhead

        // Indices for the shaft (2 triangles)
        indices.push_back(0); indices.push_back(1); indices.push_back(2);
        indices.push_back(0); indices.push_back(2); indices.push_back(3);

        // Indices for the arrowhead (1 triangle)
        indices.push_back(4); indices.push_back(5); indices.push_back(6);

        // Normalize the direction vector
        DirectX::XMVECTOR dirVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction));

        // Calculate the angle between the +X axis (default arrow direction) and the given direction
        DirectX::XMVECTOR defaultDir = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);  // Default arrow direction along +X
        float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(defaultDir, dirVec));
        float angle = acosf(dotProduct);  // Angle between default and target direction

        // Determine the rotation axis (cross product of the default direction and the given direction)
        DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(defaultDir, dirVec);

        // Apply rotation to the vertices based on the calculated axis and angle
        for (auto& vertex : vertices) {
            DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&vertex.position);
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, angle);
            posVec = DirectX::XMVector3Transform(posVec, rotationMatrix);  // Rotate the vertex
            DirectX::XMStoreFloat3(&vertex.position, posVec);
        }
        model.Initialize<Render::Vertex>(getDefaultDevice(), vertices, indices);
    }

    void Object3D::InitializeAsTexturedRectangle(float width, float height, float depth, const char* texturePath)
    {
        this->texturePath = texturePath;

        float halfWidth = width * 0.5f;
        float halfDepth = depth * 0.5f;
        float halfHeight = height * 0.5f;

        std::vector<Render::Textured::Vertex> vertices = {
            { {-halfWidth, 0.0, -halfDepth}, {0, 0} },
            { {halfWidth, 0.0, -halfDepth}, {1, 0} },
            { {halfWidth, 0.0, halfDepth}, {1, 1 } },
            { {-halfWidth, 0.0, halfDepth}, {0, 1}}
        };

        /*std::vector<Render::Textured::Vertex> vertices = {
            { {-halfWidth, -halfHeight, -halfDepth}, {0, 0} },
            { {halfWidth, -halfHeight, -halfDepth}, {1, 0} },
            { {halfWidth, halfHeight, -halfDepth}, {1, 1} },
            { {-halfWidth, halfHeight, -halfDepth}, {0, 1} }
        };*/

        std::vector<uint32_t> indices = {
            0, 1, 2, 0, 2, 3
        };

        model.Initialize<Render::Textured::Vertex>(getDefaultDevice(), vertices, indices);
    }
}
