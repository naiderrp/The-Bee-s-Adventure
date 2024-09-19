#pragma once
#include <Scene/SceneObject/Object2D.h>

namespace GameEngine {
	class HealthIndicator {
	public:
		HealthIndicator(uint32_t hitPoints, const DirectX::XMFLOAT4& color) : mIndicatorsColor(color), mHitPoints(hitPoints){
			createIndicators(mHitPoints, mIndicatorsColor, 0.12, 0.09);
		}

		void hit() {
			mIndicators.pop_back();
		}

		void reset() {
			mIndicators.clear();
			createIndicators(mHitPoints, mIndicatorsColor, 0.09, 0.09);
		}

		std::vector<GraphicsEngine::Object2D>& healthIndicators() { return mIndicators; }
	
	private:
		void createIndicators(uint32_t hitPoints, const DirectX::XMFLOAT4& color, float indicatorWidth, float indicatorHeight) {

			const float startX = -1.0f;
			const float startY = 0.9f;

			using coordinate_t = GraphicsEngine::Object2D::screen_space_coordinate_t;

			for (int i = 0; i < hitPoints; ++i) {

				float leftX = startX + i * indicatorWidth;
				float rightX = leftX + indicatorWidth;
				float topY = startY;
				float bottomY = startY - indicatorHeight;
			
				coordinate_t bottomLeft = { leftX, bottomY };
				coordinate_t topLeft = { leftX, topY };
				coordinate_t topRight = { rightX, topY };
				coordinate_t bottomRight = { rightX, bottomY };

				GraphicsEngine::Object2D indicator;
				indicator.InitializeAsRectangle(bottomLeft, topLeft, topRight, bottomRight, color);
				mIndicators.emplace_back(std::move(indicator));
			}
		}
	
	private:
		std::vector<GraphicsEngine::Object2D> mIndicators;
		const DirectX::XMFLOAT4 mIndicatorsColor;
		const uint32_t mHitPoints;
	};
}