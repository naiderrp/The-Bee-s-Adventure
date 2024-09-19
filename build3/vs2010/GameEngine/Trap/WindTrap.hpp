#pragma once
#include <Scene/SceneObject/Object3D.h>
#include <System/Timer/TimeStep.h>

#include <random>

#include <GameEngine/GameEntity/GameObject.hpp>
#include <GameEngine/GameEntity/Player.hpp>

class WindTrap {
public:
	WindTrap(float width, float height, float depth, const DirectX::XMFLOAT4& platformColor, const DirectX::XMFLOAT4& arrowColor)
		: mPlatform(width, height, depth, platformColor), mWindDirection(0, 0, -1), mState(TrapState::eIdle), mPlayer(nullptr)
	{
		mArrow.InitializeAsArrow({static_cast<float>(mWindDirection.x()), static_cast<float>(mWindDirection.y()), static_cast<float>(mWindDirection.z())}, arrowColor);
		mArrow.transform.translation.y = 0.8;
	}

	void translate(const DirectX::XMFLOAT3& translation) {

		mPlatform.transform().translation.x += translation.x;
		mPlatform.transform().translation.y += translation.y;
		mPlatform.transform().translation.z += translation.z;

		mArrow.transform.translation.x += translation.x;
		mArrow.transform.translation.y += translation.y;
		mArrow.transform.translation.z += translation.z;
	}

	void activate() {
		if (mState == TrapState::eIdle) mState = TrapState::eBlow;
	}

	void update() {
		const float interval = 2.0f;

		if (mChangeWindDirectionTimer.Countdown(interval) == 0.0f) {
			changeWindDirection();
			resetTimer(interval);
		}

		switch (mState) {
		case TrapState::eBlow:
			handleBlowing();
			break;
		}
	}

	void setPlayer(GameEngine::Player* player) { mPlayer = player; }
	
	GameEngine::GameObject& gameObject() { return mPlatform; }
	GraphicsEngine::Object3D& platformArrow() { return mArrow; }

private:
	void handleBlowing() {
		const double windStrength = 0.475;
		mPlayer->character.physicsBody()->applyCentralImpulse(mWindDirection * windStrength);
		mState = TrapState::eIdle;
	}

	void changeWindDirection() {
		const float angle = DirectX::XMConvertToRadians(static_cast<float>(rand()));
		mWindDirection = btVector3(sin(angle), 0.0, cos(angle));
		mArrow.transform.rotation.y = angle - DirectX::XM_PI;
	}

	void resetTimer(float startTime) {
		mChangeWindDirectionTimer.Countdown(startTime);
	}

private:
	enum TrapState {
		eIdle,
		eBlow
	};

	GameEngine::GameObject mPlatform;
	GraphicsEngine::Object3D mArrow;

	GraphicsEngine::Timer::TimeStep mChangeWindDirectionTimer;

	btVector3 mWindDirection;
	
	TrapState mState;

	GameEngine::Player* mPlayer;
};