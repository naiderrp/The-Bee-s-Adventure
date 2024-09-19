#pragma once
#include <System/Timer/TimeStep.h>
#include <Win32Application.h>

#include <Scene/SceneObject/Button.h>

#include "Level.hpp"

namespace GameEngine {
	class GameApplication
	{
	public:
		enum GameResult {
			eFailure,
			eSuccess
		};
	public:
		void initialize() {
			app.Initialize();
			mRestartButton.Initialize({ -0.25, -0.5 }, { 0.15, -0.5 }, { 0.15, 0.1 }, { -0.25, 0.1 });
		}

		void renderScene() {
			if (mRestartButton.Clicked()) restartGameApplication();

			auto dt = mTimer.Tick();
			
			mScene->update(dt);
			
			app.Update();
		}

		void setScene(GameEngine::Level& scene) {
			mScene = &scene;
			app.renderer.setScene(mScene->graphicsScene());
			mState = GameState::eRunning;
		}

		bool isRunning() { return app.isRunning(); }
		
		void gameOver(GameResult result) {
			if (mState != GameState::eRunning) return;
			switch (result) {
			case GameResult::eFailure:
				handleFailure();
				break;
			case GameResult::eSuccess:
				handleSuccess();
				break;
			}
			mScene->graphicsScene().setButton(mRestartButton);
			mState = GameState::eIdle;
		}

	private:
		void handleFailure() {
			mGameResult.DrawInScreenSpace(true);
			mGameResult.text = L"Поражение!";
			mGameResult.pos = { 0.35, 0.25 };
			mScene->graphicsScene().addTextObject(mGameResult);
		}

		void handleSuccess() {
			mGameResult.DrawInScreenSpace(true);
			mGameResult.scale = { 4.0, 4.0 };
			mGameResult.pos = { 0.0, 0.15 };
			auto elapsedTime = std::to_wstring(mScene->getGamePlayTime());
			for(int i = 0; i < 4; ++i) elapsedTime.pop_back();

			mGameResult.text = L"Победа!nВремя прохождения: " + elapsedTime + L" секунд";
			
			mScene->graphicsScene().addTextObject(mGameResult);
		}

		void restartGameApplication() {
			mScene->restart();
			mScene->graphicsScene().resetButton();
			mScene->graphicsScene().popTextObject();
			
			mState = GameState::eRunning;
		}

	private:
		enum GameState {
			eRunning,
			eIdle
		};

	private:
		GraphicsEngine::Timer::TimeStep mTimer;
		GraphicsEngine::Win32Application app;

		GameEngine::Level* mScene = nullptr;

		GraphicsEngine::ScreenText mGameResult;

		GraphicsEngine::Button mRestartButton;

		GameState mState = GameState::eIdle;
	};
}