#pragma once
#include <pch.h>
#include <chrono>

namespace GraphicsEngine {
	namespace Timer {
		class GRAPHICS_API TimeStep
		{
		public:
			TimeStep();
			float Countdown(float startTime);
			float Tick();
			float GetElapsedTime() const;
			void Restart();
			void ResetCountdownTimer();
		private:
			std::chrono::steady_clock::time_point mLastTime;
			float mRemainingTime = 0.0f;
		};
	}
}