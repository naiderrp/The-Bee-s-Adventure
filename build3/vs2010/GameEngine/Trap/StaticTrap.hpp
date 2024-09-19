#pragma once
#include <GameEngine/GameEntity/GameObject.hpp>

class StaticTrap {
public:
	StaticTrap(float width, float height, float depth, const DirectX::XMFLOAT4& color)
		: mPlatform(width, height, depth, color)
	{
	}

	void translate(const DirectX::XMFLOAT3& translation) {
		mPlatform.transform().translation.x += translation.x;
		mPlatform.transform().translation.y += translation.y;
		mPlatform.transform().translation.z += translation.z;
	}

	GameEngine::GameObject& gameObject() { return mPlatform; }

private:
	GameEngine::GameObject mPlatform;
};