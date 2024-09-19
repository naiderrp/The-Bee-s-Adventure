#pragma once
#include <GameEngine/GameEntity/GameObject.hpp>

class TrampolineTrap {
public:
	TrampolineTrap(float width, float height, float depth, const DirectX::XMFLOAT4& color, float jumpFactor)
		: mPlatform(width, height, depth, color), mPlayer(nullptr), mJumpFactor(jumpFactor)
	{
		mTrampolineTexture.InitializeAsTexturedRectangle(width, 0.0, depth);
		mTrampolineTexture.transform.translation.y = height / 2.0 + 0.05;
	}

	void activate() {
		mPlayer->character.physicsBody()->applyCentralImpulse({ 0.0, 1.0 * mJumpFactor, 0.0 });
	}

	void translate(const DirectX::XMFLOAT3 &translation) {
		mPlatform.transform().translation.x += translation.x;
		mPlatform.transform().translation.y += translation.y;
		mPlatform.transform().translation.z += translation.z;

		mTrampolineTexture.transform.translation.x += translation.x;
		mTrampolineTexture.transform.translation.y += translation.y;
		mTrampolineTexture.transform.translation.z += translation.z;
	}

	void setPlayer(GameEngine::Player* player) { mPlayer = player; }

	GameEngine::GameObject& gameObject() { return mPlatform; }
	GraphicsEngine::Object3D& trampolineGraphicsObject() { return mTrampolineTexture; }

private:
	GameEngine::GameObject mPlatform;
	GraphicsEngine::Object3D mTrampolineTexture;

	GameEngine::Player* mPlayer;

	const float mJumpFactor;
};