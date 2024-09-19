#include "pch.h"
#include "TimeStep.h"

namespace GraphicsEngine {
	namespace Timer {
		TimeStep::TimeStep()
		{
			mLastTime = std::chrono::steady_clock::now();
		}
		float TimeStep::Countdown(float startTime)
		{
			if (mRemainingTime <= 0.0f) {
				mLastTime = std::chrono::steady_clock::now();
				mRemainingTime = startTime;
			}

			float deltaTime = Tick();
			mRemainingTime -= deltaTime;

			if (mRemainingTime < 0.0f) {
				mRemainingTime = 0.0f;
			}

			return mRemainingTime;
		}
		float TimeStep::Tick()
		{
			const std::chrono::steady_clock::time_point oldTime = mLastTime;
			mLastTime = std::chrono::steady_clock::now();

			const std::chrono::duration<float> frameTime = mLastTime - oldTime;

			return frameTime.count();
		}
		float TimeStep::GetElapsedTime() const 
		{
			const std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
			const std::chrono::duration<float> elapsedTime = currentTime - mLastTime;
			return elapsedTime.count();
		}
		void TimeStep::Restart()
		{
			mLastTime = std::chrono::steady_clock::now();
		}
		void TimeStep::ResetCountdownTimer()
		{
			mRemainingTime = 0.0f;
		}
	}
}