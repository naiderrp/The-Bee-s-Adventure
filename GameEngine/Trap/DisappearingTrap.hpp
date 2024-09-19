#pragma once
#include <GameEngine/GameEntity/GameObject.hpp>
#include <GameEngine/GameEntity/PhysicsWorld.hpp>

class DisappearingTrap {
public:
	enum TrapState {
		eIdle,
		eChangeColor,
		eFading,
		eDisappeared
	};
public:
	DisappearingTrap(float width, float height, float depth, const DirectX::XMFLOAT4& platformColor, const DirectX::XMFLOAT4& activateColor = GameEngine::Colors::CYAN)
		: mPlatform(width, height, depth, platformColor), mState(TrapState::eIdle), mInitColor(platformColor), mActivateColor(activateColor)
	{
	}

	void activate() {
		if (mState == TrapState::eIdle) mState = TrapState::eChangeColor;
	}

	void update() {
		switch (mState) {
		case TrapState::eChangeColor:
			handleChangingColor();
			break;
		case TrapState::eFading:
			handleFading();
			break;
		case TrapState::eDisappeared:
			handleDisappearing();
			break;
		}
	}

	void translate(const DirectX::XMFLOAT3& translation) {
		mPlatform.transform().translation.x += translation.x;
		mPlatform.transform().translation.y += translation.y;
		mPlatform.transform().translation.z += translation.z;

		mInitPosition = mPlatform.transform().translation;
	}

	void reset() {
		auto position = mPlatform.transform().translation;
		position.y += 100.0;
		mPlatform.setPos(mInitPosition);
		mPlatform.color() = mInitColor;
		mState = TrapState::eIdle;
	}

	GameEngine::GameObject& gameObject() { return mPlatform; }

	TrapState currentState() const { return mState; }

	DirectX::XMFLOAT4 initColor() const { return mInitColor; }

	DirectX::XMFLOAT3& initPosition() { return mInitPosition; }

private:
	void handleChangingColor() {
		mPlatform.color() = mActivateColor;
		mState = TrapState::eFading;
	}

	void handleFading() {
		const float disappearingSpeed = 0.99;
		mPlatform.color().w *= disappearingSpeed;

		if (mPlatform.color().w <= 0.02) mState = TrapState::eDisappeared;
	}

	void handleDisappearing() {
		auto trapPosition = mPlatform.transform().translation;
		mPlatform.setPos({ trapPosition.x, trapPosition.y - 100.0f, trapPosition.z });

		mPlatform.color().w = 0.0f;
		mState = TrapState::eIdle;
	}

private:
	GameEngine::GameObject mPlatform;
	TrapState mState;
	DirectX::XMFLOAT4 mInitColor;
	DirectX::XMFLOAT4 mActivateColor;
	DirectX::XMFLOAT3 mInitPosition;
};