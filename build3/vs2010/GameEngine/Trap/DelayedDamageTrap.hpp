#pragma once

#include <GameEngine/GameEntity/GameObject.hpp>

#include <System/Timer/TimeStep.h>
#include <Scene/SceneObject/ScreenText.h>

#include "Player.hpp"

class DelayedDamageTrap {
public:
	DelayedDamageTrap(float width, float height, float depth, float cooldown, float delay, const DirectX::XMFLOAT4& initColor = GameEngine::Colors::WHITE, const DirectX::XMFLOAT4& activateColor = GameEngine::Colors::TANGERINE_ORANGE)
		: mInitColor(initColor), mActivateColor(activateColor), mPlatform(width, height, depth, mInitColor), mCooldown(cooldown), mDelay(delay)
	{
		mRemainingTime.font = L"fonts/Arial.fnt";
		mRemainingTime.scale = { 20.0, 20.0 };
	}

	GameEngine::GameObject& gameObject() { return mPlatform; }

	void translate(const DirectX::XMFLOAT3& translation) {
		mPlatform.transform().translation.x += translation.x;
		mPlatform.transform().translation.y += translation.y;
		mPlatform.transform().translation.z += translation.z;

		mRemainingTime.textTransformComponent.translation.x += translation.x;
		mRemainingTime.textTransformComponent.translation.y += translation.y;
		mRemainingTime.textTransformComponent.translation.z += translation.z;
		
		mRemainingTime.textTransformComponent.translation.y += 2.0f;
	}

	void activate() {
		if (mState == TrapState::eDamaging) damagePlayer();
		if (mState != TrapState::eIdle) return;
		mState = TrapState::eActivated;
		mPlatform.color() = mActivateColor;
	}

	void update() {
		switch (mState) {
		case TrapState::eActivated:
			handleActivation();
			break;
		case TrapState::eBlink:
			handleBlinking(0.2f);
			break;
		case TrapState::eDamaging:
			handleDamaging();
			break;
		case TrapState::eCooldown:
			handleCooldown();
			break;
		}
	}

	void setPlayer(GameEngine::Player* player) { mPlayer = player; }

	GraphicsEngine::ScreenText& remainingTime() { return mRemainingTime; }
	
	void reset() {
		mDelayTimer.ResetCountdownTimer();
		mCooldownTimer.ResetCountdownTimer();
		mBlinkTimer.ResetCountdownTimer();

		mState = TrapState::eIdle;
		mRemainingTime.text = L"";
		mPlatform.color() = mInitColor;
	}

	bool isActivated() const { return mState == TrapState::eActivated; }

private:
	void handleActivation() {
		float remainingTime = mDelayTimer.Countdown(mDelay);
		mRemainingTime.text = std::to_wstring(remainingTime);

		if (remainingTime == 0.0f)
		{
			mState = TrapState::eBlink;
		}
	}

	void handleBlinking(float blinkDuration) {
		mPlatform.color() = mDamageColor;
		
		float remainingTime = mBlinkTimer.Countdown(blinkDuration);
		
		if (remainingTime == 0.0)
		{
			mState = TrapState::eDamaging;
		}
	}

	void handleDamaging() {
		mState = TrapState::eCooldown;
	}

	void handleCooldown() {
		// reset 
		{
			mRemainingTime.text = L"";
			mPlatform.color() = mInitColor;
		}
		float remainingTime = mCooldownTimer.Countdown(mCooldown);

		if (remainingTime == 0.0f)
		{
			mState = TrapState::eIdle;
		}
	}

	void damagePlayer() {
		mPlayer->characterHealthIndicator.hit();
	}

private:
	enum TrapState {
		eIdle,
		eActivated,
		eBlink,
		eDamaging,
		eCooldown
	};

private:
	GraphicsEngine::ScreenText mRemainingTime;

	float mCooldown;
	float mDelay;

	GraphicsEngine::Timer::TimeStep mDelayTimer;
	GraphicsEngine::Timer::TimeStep mCooldownTimer;
	GraphicsEngine::Timer::TimeStep mBlinkTimer;
	
	DirectX::XMFLOAT4 mActivateColor;
	DirectX::XMFLOAT4 mInitColor;
	DirectX::XMFLOAT4 mDamageColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	GameEngine::GameObject mPlatform;

	TrapState mState = TrapState::eIdle;

	GameEngine::Player* mPlayer = nullptr;
};