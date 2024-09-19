#pragma once
#include "pch.h"

#include "RenderAPI/DirectX12/Resource/D12Resource.h"
#include "RenderAPI/DirectX12/Pipeline/D12PipelineState.h"

namespace GraphicsEngine {
	namespace Render {
		struct Vertex {
			Vertex() = default;
			Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 1.0f, 1.0f }) : position(position), color(color)
			{}

			Vertex(const Vertex&) = default;
			Vertex(Vertex&&) = default;

			bool operator==(const Vertex& other) const {
				return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z &&
					color.x == other.color.x && color.y == other.color.y && color.z == other.color.z && color.w == other.color.w;
			}

			static D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() {
				static D3D12_INPUT_ELEMENT_DESC elements[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Render::Vertex, color),
					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				};

				D3D12_INPUT_LAYOUT_DESC inputLayout = {};
				inputLayout.NumElements = _countof(elements);
				inputLayout.pInputElementDescs = elements;

				return inputLayout;
			}

			DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		};

		struct PassData {
			DirectX::XMMATRIX MVP = DirectX::XMMatrixIdentity();
			DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		};

		namespace SkyBox {
			struct Vertex {
				static D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() {
					static D3D12_INPUT_ELEMENT_DESC elements[] = {
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 }
					};

					D3D12_INPUT_LAYOUT_DESC inputLayout = {};
					inputLayout.NumElements = _countof(elements);
					inputLayout.pInputElementDescs = elements;

					return inputLayout;
				}
				DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			};

			struct PassData {
				DirectX::XMMATRIX viewProjectionMatrix = DirectX::XMMatrixIdentity();
				DirectX::XMFLOAT4 cameraPositionAndScaleFactor = { 0.0f, 0.0f, 0.0f, 1.0f }; // cameraPosition.w is a scale factor
			};
		}
	
		namespace Text {
			// https://www.braynzarsoft.net/viewtutorial/q16390-11-drawing-text-in-directx-12
			struct Vertex {
				Vertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h) : color(r, g, b, a), texCoord(u, v, tw, th), pos(x, y, w, h) {}
				
				static D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() {
					static D3D12_INPUT_ELEMENT_DESC elements[] =
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
						{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
					};

					D3D12_INPUT_LAYOUT_DESC inputLayout = {};
					inputLayout.NumElements = _countof(elements);
					inputLayout.pInputElementDescs = elements;

					return inputLayout;
				}
				
				DirectX::XMFLOAT4 pos;
				DirectX::XMFLOAT4 texCoord;
				DirectX::XMFLOAT4 color;
			};
			struct FontChar
			{
				// the unicode id
				int id;

				// these need to be converted to texture coordinates 
				// (where 0.0 is 0 and 1.0 is textureWidth of the font)
				float u; // u texture coordinate
				float v; // v texture coordinate
				float twidth; // width of character on texture
				float theight; // height of character on texture

				float width; // width of character in screen coords
				float height; // height of character in screen coords

				// these need to be normalized based on size of font
				float xoffset; // offset from current cursor pos to left side of character
				float yoffset; // offset from top of line to top of character
				float xadvance; // how far to move to right for next character
			};
			struct FontKerning
			{
				int firstid; // the first character
				int secondid; // the second character
				float amount; // the amount to add/subtract to second characters x
			};
			struct Font
			{
				std::wstring name; // name of the font
				std::wstring fontImage;
				int size; // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
				float lineHeight; // how far to move down to next line, will be normalized
				float baseHeight; // height of all characters, will be normalized
				int textureWidth; // width of the font texture
				int textureHeight; // height of the font texture
				int numCharacters; // number of characters in the font
				FontChar* CharList; // list of characters
				int numKernings; // the number of kernings
				FontKerning* KerningsList; // list to hold kerning values
				ID3D12Resource* textureBuffer; // the font texture resource
				D3D12_GPU_DESCRIPTOR_HANDLE srvHandle; // the font srv

				// these are how much the character is padded in the texture. We
				// add padding to give sampling a little space so it does not accidentally
				// padd the surrounding characters. We will need to subtract these paddings
				// from the actual spacing between characters to remove the gaps you would otherwise see
				float leftpadding;
				float toppadding;
				float rightpadding;
				float bottompadding;

				// this will return the amount of kerning we need to use for two characters
				float GetKerning(wchar_t first, wchar_t second)
				{
					for (int i = 0; i < numKernings; ++i)
					{
						if ((wchar_t)KerningsList[i].firstid == first && (wchar_t)KerningsList[i].secondid == second)
							return KerningsList[i].amount;
					}
					return 0.0f;
				}

				// this will return a FontChar given a wide character
				FontChar* GetChar(wchar_t c)
				{
					for (int i = 0; i < numCharacters; ++i)
					{
						if (c == (wchar_t)CharList[i].id)
							return &CharList[i];
					}
					return nullptr;
				}
			};

			struct GraphicsContext {
				D12PipelineState &textPipeline;
				D12Resource &textPassData;
				ID3D12Resource* textVertexBuffer;
				ID3D12DescriptorHeap* textDescriptorHeap;
				D3D12_VERTEX_BUFFER_VIEW* textVertexBufferView;
				UINT8** textVBGPUAddress;
			};
		}
	
		namespace Textured {
			struct Vertex {
				Vertex() = default;
				Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& uv) : position(position), uv(uv)
				{}

				Vertex(const Vertex&) = default;
				Vertex(Vertex&&) = default;

				static D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() {
					static D3D12_INPUT_ELEMENT_DESC elements[] = {
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Render::Textured::Vertex, uv),
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
					};

					D3D12_INPUT_LAYOUT_DESC inputLayout = {};
					inputLayout.NumElements = _countof(elements);
					inputLayout.pInputElementDescs = elements;

					return inputLayout;
				}

				DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
				DirectX::XMFLOAT2 uv = { 1.0f, 1.0f };
			};

			struct PassData {
				DirectX::XMMATRIX MVP = DirectX::XMMatrixIdentity();
			};
		}
	
		namespace Drawing2D{
		struct Vertex {
			Vertex() = default;
			Vertex(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 1.0f, 1.0f }) : position(position), color(color)
			{}

			Vertex(const Vertex&) = default;
			Vertex(Vertex&&) = default;

			static D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() {
				static D3D12_INPUT_ELEMENT_DESC elements[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Render::Drawing2D::Vertex, color),
					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				};

				D3D12_INPUT_LAYOUT_DESC inputLayout = {};
				inputLayout.NumElements = _countof(elements);
				inputLayout.pInputElementDescs = elements;

				return inputLayout;
			}

			DirectX::XMFLOAT2 position = { 0.0f, 0.0f };
			DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		};
		namespace Textured {
			struct Vertex {
				Vertex() = default;
				Vertex(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& uv) : position(position), uv(uv)
				{}

				Vertex(const Vertex&) = default;
				Vertex(Vertex&&) = default;

				static D3D12_INPUT_LAYOUT_DESC getInputLayoutDesc() {
					static D3D12_INPUT_ELEMENT_DESC elements[] = {
						{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Render::Drawing2D::Textured::Vertex, uv),
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
					};

					D3D12_INPUT_LAYOUT_DESC inputLayout = {};
					inputLayout.NumElements = _countof(elements);
					inputLayout.pInputElementDescs = elements;

					return inputLayout;
				}

				DirectX::XMFLOAT2 position = { 0.0f, 0.0f };
				DirectX::XMFLOAT2 uv = { 0.0f, 0.0f };
			};
		}
		}
	}
}

template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

namespace std {
	template <>
	struct hash<DirectX::XMFLOAT3> {
		size_t operator()(DirectX::XMFLOAT3 const& vec) const {
			size_t seed = 0;
			hashCombine(seed, vec.x, vec.y, vec.z);
			return seed;
		}
	};

	template <>
	struct hash<DirectX::XMFLOAT4> {
		size_t operator()(DirectX::XMFLOAT4 const& vec) const {
			size_t seed = 0;
			hashCombine(seed, vec.x, vec.y, vec.z, vec.w);
			return seed;
		}
	};

	template <>
	struct hash<GraphicsEngine::Render::Vertex> {
		size_t operator()(GraphicsEngine::Render::Vertex const& vertex) const {
			size_t seed = 0;
			hashCombine(seed, vertex.position, vertex.color);
			return seed;
		}
	};
}  // namespace std
