#pragma once
#include "Object2D.h"

namespace GraphicsEngine {
	class Button
	{
	public:
		inline void Initialize(Object2D::screen_space_coordinate_t bottomLeft, Object2D::screen_space_coordinate_t bottomRight, Object2D::screen_space_coordinate_t topRight, Object2D::screen_space_coordinate_t topLeft) {
			mSquare.InitializeAsTexturedRectangle(bottomLeft, bottomRight, topRight, topLeft);

			this->bottomLeft = bottomLeft;
			this->topLeft = topLeft;
			this->topRight = topRight;
			this->bottomRight = bottomRight;
		}

		inline bool Clicked() const { return mClicked; }
		
		inline void Click(bool on = true) { mClicked = on; }
	
		inline Object2D& graphicsObject() { return mSquare; }

	public:
		DirectX::XMFLOAT2 bottomLeft;
		DirectX::XMFLOAT2 topLeft;
		DirectX::XMFLOAT2 topRight;
		DirectX::XMFLOAT2 bottomRight;

	private:
		Object2D mSquare;
		bool mClicked = false;
	};
}
