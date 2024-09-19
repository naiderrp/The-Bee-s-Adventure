#include <GameEngine/GameEntity/GameApplication.hpp>
#include <GameEngine/GameEntity/Level.hpp>

template<typename Trap>
void lineUp(std::vector<Trap>& traps, size_t firstElement, size_t count, DirectX::XMFLOAT3 startPos, float padding) {
	
	DirectX::XMFLOAT3 currentPos = startPos;
	
	size_t index = firstElement;
	
	for (size_t i = 0; i < count; ++i) {
		
		traps[index].translate(currentPos);
		currentPos.x += traps[index].gameObject().width() + padding;
	
		++index;
	}
}
template<typename Trap>
void arrangeTrapsInDiamond(std::vector<Trap>& traps, std::vector<size_t> trapsPerRow, DirectX::XMFLOAT3 startPos, float rowPadding, float trapPadding) {
	size_t totalTrapsUsed = 0;
	DirectX::XMFLOAT3 currentPos = startPos;

	for (size_t row = 0; row < trapsPerRow.size(); ++row) {
		size_t trapsInRow = trapsPerRow[row];

		DirectX::XMFLOAT3 rowStartPos = currentPos;
		rowStartPos.x -= (trapsInRow * (traps[totalTrapsUsed].gameObject().width() + trapPadding)) / 2.0f;

		lineUp(traps, totalTrapsUsed, trapsInRow, rowStartPos, trapPadding);

		currentPos.z += traps[totalTrapsUsed].gameObject().width() + rowPadding;

		totalTrapsUsed += trapsInRow;
	}
}

int main() {
	GameEngine::GameApplication app;
	app.initialize();

	GameEngine::GameObject bee = { "models/bee.obj", GameEngine::GameObject::BoundingBox{1.5f, 1.5f, 1.5f} };
	bee.transform().translation.y = 1.75f;
	
	GameEngine::TexturedPlatform startPlatform = { 10.0f, 1.0, 10.0f, 1.0, 1.0, GameEngine::Colors::YELLOW };

	std::vector<StaticTrap> staticTraps;
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::FUCHSIA);
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::TURQUOISE);
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::LIGHT_PURPLE);
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::FUCHSIA);
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::FUCHSIA);
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::TURQUOISE);
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::LIGHT_PURPLE);
	for (int i = 0; i < 3; ++i) staticTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::FUCHSIA);

	const float padding = 3.0;
	const float xPos = -6.0;

	lineUp(staticTraps, 0, 3, { xPos, 0.0, 9.0 }, padding);

	std::vector<DelayedDamageTrap> delayedDamageTraps;
	for (int i = 0; i < 6; ++i) delayedDamageTraps.emplace_back(3.0, 1.0, 3.0, 5.0, 1.0, GameEngine::Colors::DARK_BLUE, GameEngine::Colors::TANGERINE_ORANGE);

	lineUp(delayedDamageTraps, 0, 3, { xPos, 0.0, 15.0 }, padding);
	lineUp(delayedDamageTraps, 3, 3, { xPos, 0.0, 21.0 }, padding);

	lineUp(staticTraps, 3, 3, { xPos, 0.0, 27.0 }, padding);
	lineUp(staticTraps, 6, 3, { xPos, 2.5, 33.0 }, padding);
	lineUp(staticTraps, 9, 3, { xPos, 5.0, 39.0 }, padding);

	SpinningTrap spinningTrap = { 20.0f, 1.0, 2.0f, GameEngine::Colors::MINT, GameEngine::Colors::DARK_PURPLE };
	spinningTrap.translate({ 0.0f, 5.0f, 53.0f });

	lineUp(staticTraps, 12, 3, { xPos, 5.0, 65.0 }, padding);

	std::vector<WindTrap> windTraps;
	for (int i = 0; i < 6; ++i) windTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::DARK_BLUE, GameEngine::Colors::YELLOW);

	lineUp(windTraps, 0, 3, { xPos, 5.0, 71.0 }, padding);
	lineUp(windTraps, 3, 3, { xPos, 5.0, 77.0 }, padding);

	lineUp(staticTraps, 15, 3, { xPos, 5.0, 83.0 }, padding);
	lineUp(staticTraps, 18, 3, { xPos, 7.5, 89.0 }, padding);
	lineUp(staticTraps, 21, 3, { xPos, 10.0, 95.0 }, padding);

	std::vector<DisappearingTrap> disappearingTraps;
	for (int i = 0; i < 73; ++i) disappearingTraps.emplace_back(1.5, 0.5, 1.5, GameEngine::Colors::TURQUOISE);

	arrangeTrapsInDiamond(disappearingTraps, { 3, 7, 9, 11, 13, 11, 9, 7, 3 }, { 0.0, 10.0, 99 }, 0.5, 0.5);
	
	std::vector<TrampolineTrap> trampolineTraps;
	for (int i = 0; i < 3; ++i) trampolineTraps.emplace_back(3.0, 1.0, 3.0, GameEngine::Colors::DARK_BLUE, 35.0);

	lineUp(trampolineTraps, 0, 3, { xPos, 10.0, 120.0 }, padding);

	GameEngine::TexturedPlatform finishPlatform = { 10.0f, 1.0f, 10.0f, -3.0, 1.0, GameEngine::Colors::YELLOW };
	finishPlatform.translate({ 0.0, 15.0, 129.5 });

	GraphicsEngine::SkyBox skybox = {};
	skybox.InitializeAsSphere(0.5f);
	skybox.texturePath = L"textures\\skybox\\gradient_skybox.dds";
	skybox.scaleFactor = 20.0f;

	GraphicsEngine::Camera thirdPersonCamera;
	GameEngine::CharacterMovementController beeController = {bee, thirdPersonCamera };
	GameEngine::HealthIndicator beeHealth = { 5, GameEngine::Colors::LIME_GREEN };
	GameEngine::Player player = { bee, beeController, beeHealth, thirdPersonCamera};

	GameEngine::Level mainScene = {};

	mainScene.setSkyBox(skybox);

	for(auto& trap : delayedDamageTraps) mainScene.addDelayedDamageTrap(trap);
	
	for(auto& trap : windTraps) mainScene.addWindTrap(trap);
	
	for(auto& trap : trampolineTraps) mainScene.addTrampolineTrap(trap);
	
	for(auto& trap : disappearingTraps) mainScene.addDisappearingTrap(trap);

	mainScene.addSpinningTrap(spinningTrap);

	for (auto& trap : staticTraps) mainScene.addStaticTrap(trap);

	mainScene.setStartPlatform(startPlatform);
	mainScene.setFinishPlatform(finishPlatform);

	mainScene.setPlayer(player);

	app.setScene(mainScene);

	while (app.isRunning()) {

		app.renderScene();

		if (beeHealth.healthIndicators().empty())
		{
			app.gameOver(GameEngine::GameApplication::GameResult::eFailure);
		}
		else if (mainScene.isVictory())
		{
			app.gameOver(GameEngine::GameApplication::GameResult::eSuccess);
		}
	}

	return 0;
}