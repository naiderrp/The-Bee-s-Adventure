#pragma once
#include "GameObject.hpp"
#include "CharacterMovementController.hpp"
#include "HealthIndicator.hpp"
#include <Scene/Camera.h>

namespace GameEngine {
	struct Player {
		GameEngine::GameObject& character;
		GameEngine::CharacterMovementController& characterController;
		GameEngine::HealthIndicator& characterHealthIndicator;
		GraphicsEngine::Camera& camera;
	};
}