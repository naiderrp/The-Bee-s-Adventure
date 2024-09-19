#pragma once

#include <GameEngine/GameEntity/GameObject.hpp>

class SpinningTrap {
public:
	SpinningTrap(float width, float height, float depth, const DirectX::XMFLOAT4 spinningBarColor, const DirectX::XMFLOAT4& platformsColor) 
		: mSpinningBar(width, height, depth, spinningBarColor)
	{
		initPlatforms(platformsColor, spinningBarColor, mSpinningBar.transform().translation, 9, 6.0f);
		mSpinningBar.transform().translation.y = 2.0;
	}

	void translate(const DirectX::XMFLOAT3& translation) {
		mSpinningBar.transform().translation.x += translation.x;
		mSpinningBar.transform().translation.y += translation.y;
		mSpinningBar.transform().translation.z += translation.z;

		for (auto& platform : mPlatforms) {
			platform.transform().translation.x += translation.x;
			platform.transform().translation.y += translation.y;
			platform.transform().translation.z += translation.z;
		}
	}

	void spin(float dt, float rotationSpeed) {
		mSpinningBar.transform().rotation.y += rotationSpeed * dt; // Update rotation around the Y-axis (XZ plane)
		
		auto transform = mSpinningBar.physicsBody()->getWorldTransform();

		btQuaternion quat;
		quat.setEuler(mSpinningBar.transform().rotation.y, 0.0, 0.0);
		transform.setRotation(quat);

		mSpinningBar.physicsBody()->getMotionState()->setWorldTransform(transform);

	}

	GameEngine::GameObject& spinningBlockGameObject() { return mSpinningBar; }
	std::vector<GameEngine::GameObject>& platformsGameObjects() { return mPlatforms; }

private:
	void initPlatforms(const DirectX::XMFLOAT4& color, DirectX::XMFLOAT4 barPlatformColor, const DirectX::XMFLOAT3& barCenter, const int platformCount, const float spacing) {
		
		const float width = 3.0f;
		const float height = 1.0f;
		const float depth = 3.0f;

		int gridSize = static_cast<int>(std::ceil(std::sqrt(platformCount)));

		float offsetX = (gridSize - 1) * spacing * 0.5f;
		float offsetZ = (gridSize - 1) * spacing * 0.5f;

		int placedPlatforms = 0;

		int centerRow = gridSize / 2;
		int centerCol = gridSize / 2;

		for (int row = 0; row < gridSize && placedPlatforms < platformCount; ++row) {
			for (int col = 0; col < gridSize && placedPlatforms < platformCount; ++col) {
				DirectX::XMFLOAT3 translation = {
					barCenter.x + col * spacing - offsetX,
					barCenter.y,                          
					barCenter.z + row * spacing - offsetZ
				};

				if (row == centerRow && col == centerCol) 
					mPlatforms.emplace_back(width, width, depth, barPlatformColor);
				else 
					mPlatforms.emplace_back(width, height, depth, color);
				
				mPlatforms.back().transform().translation = translation;

				++placedPlatforms;
			}
		}
	}

private:
	GameEngine::GameObject mSpinningBar;
	std::vector<GameEngine::GameObject> mPlatforms;
};