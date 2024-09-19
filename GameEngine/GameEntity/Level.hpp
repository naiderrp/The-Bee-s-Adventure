#pragma once
#include <Scene/GraphicsScene.h>
#include <Scene/Camera.h>

#include <GameEngine/Trap/StaticTrap.hpp>
#include <GameEngine/Trap/DelayedDamageTrap.hpp>
#include <GameEngine/Trap/SpinningTrap.hpp>
#include <GameEngine/Trap/WindTrap.hpp>
#include <GameEngine/Trap/DisappearingTrap.hpp>
#include <GameEngine/Trap/TrampolineTrap.hpp>

#include <GameEngine/Trap/TexturedPlatform.hpp>

#include "Player.hpp"

#include <ranges>

namespace GameEngine {
	void CharacterCollidedTickCallback(btDynamicsWorld* dynamicsWorld, btScalar timeStep);

	class Level
	{
	public:
		void setStartPlatform(TexturedPlatform& platform) {
			mPhysicsWorld.addPhysicsObject(platform.gameObject(), PhysicsWorld::PhysicsObjectType::eStatic);
			platform.gameObject().physicsBody()->setUserIndex(mGraphicsScene.sceneObjects.size());
			mGraphicsScene.addSceneObject(platform.gameObject().graphicsMesh());

			platform.initLinePhysicsBody(mPhysicsWorld, 1.425);
			platform.linePhysicsBody()->setUserIndex(LevelObjectIndex::eStartLine);
			mGraphicsScene.addTextureObject(platform.textureGraphicsObject());
		}

		void setFinishPlatform(TexturedPlatform& platform) {
			mPhysicsWorld.addPhysicsObject(platform.gameObject(), PhysicsWorld::PhysicsObjectType::eStatic);
			platform.gameObject().physicsBody()->setUserIndex(mGraphicsScene.sceneObjects.size());
			mGraphicsScene.addSceneObject(platform.gameObject().graphicsMesh());

			platform.initLinePhysicsBody(mPhysicsWorld);
			platform.linePhysicsBody()->setUserIndex(LevelObjectIndex::eFinishLine);
			mGraphicsScene.addTextureObject(platform.textureGraphicsObject());
		}

		void addDelayedDamageTrap(DelayedDamageTrap& trap) {
			mGraphicsScene.addSceneObject(trap.gameObject().graphicsMesh());
			mGraphicsScene.addTextObject(trap.remainingTime());
			mPhysicsWorld.addPhysicsObject(trap.gameObject(), PhysicsWorld::PhysicsObjectType::eStatic);
			
			trap.setPlayer(mPlayer);
			
			trap.gameObject().physicsBody()->setUserIndex(mDelayedDamageTraps.size());
			
			mDelayedDamageTraps.emplace_back(trap);
		}

		void addSpinningTrap(SpinningTrap& trap)
		{
			mGraphicsScene.addSceneObject(trap.spinningBlockGameObject().graphicsMesh());
			mPhysicsWorld.addPhysicsObject(trap.spinningBlockGameObject(), PhysicsWorld::PhysicsObjectType::eKinematic);

			trap.spinningBlockGameObject().physicsBody()->setCollisionFlags(trap.spinningBlockGameObject().physicsBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			trap.spinningBlockGameObject().physicsBody()->setActivationState(DISABLE_DEACTIVATION);

			for (auto& block : trap.platformsGameObjects()) {
				mGraphicsScene.addSceneObject(block.graphicsMesh());
				mPhysicsWorld.addPhysicsObject(block, PhysicsWorld::PhysicsObjectType::eStatic);
			}

			mSpinningTraps.emplace_back(trap);
		}

		void addWindTrap(WindTrap& trap)
		{
			mGraphicsScene.addSceneObject(trap.gameObject().graphicsMesh());
			mPhysicsWorld.addPhysicsObject(trap.gameObject(), PhysicsWorld::PhysicsObjectType::eStatic);

			mWindTraps.emplace_back(trap);
			
			trap.gameObject().physicsBody()->setUserIndex(mDelayedDamageTraps.size() - 1 + mWindTraps.size());
			
			mGraphicsScene.addSceneObject(trap.platformArrow());
		}

		void addTrampolineTrap(TrampolineTrap& trap)
		{
			mGraphicsScene.addSceneObject(trap.gameObject().graphicsMesh());
			mPhysicsWorld.addPhysicsObject(trap.gameObject(), PhysicsWorld::PhysicsObjectType::eStatic);

			mGraphicsScene.addTextureObject(trap.trampolineGraphicsObject());

			trap.gameObject().physicsBody()->setUserIndex(mDelayedDamageTraps.size() + mWindTraps.size() + mTrampolineTraps.size());
			
			mTrampolineTraps.emplace_back(&trap);
		}

		void addDisappearingTrap(DisappearingTrap& trap)
		{
			trap.initPosition() = trap.gameObject().transform().translation;
			mGraphicsScene.addSceneObject(trap.gameObject().graphicsMesh());
			mPhysicsWorld.addPhysicsObject(trap.gameObject(), PhysicsWorld::PhysicsObjectType::eStatic);
			
			trap.gameObject().physicsBody()->setUserIndex(mDelayedDamageTraps.size() + mWindTraps.size() + mTrampolineTraps.size() + mDisappearingTraps.size());
			
			mDisappearingTraps.emplace_back(&trap);
		}

		void addStaticTrap(StaticTrap& platform)
		{
			mGraphicsScene.addSceneObject(platform.gameObject().graphicsMesh());
			mPhysicsWorld.addPhysicsObject(platform.gameObject(), PhysicsWorld::PhysicsObjectType::eStatic);
		}

		void setPlayer(Player& player)
		{
			mGraphicsScene.addSceneObject(player.character.graphicsMesh());
			mPhysicsWorld.addPhysicsObject(player.character, PhysicsWorld::PhysicsObjectType::eDynamic);

			player.character.physicsBody()->setAngularFactor(0.0);
			player.character.physicsBody()->setActivationState(DISABLE_DEACTIVATION);
			player.character.physicsBody()->setMotionState(mPhysicsWorld.motionStates().back().get());
			player.character.physicsBody()->setDamping(0.9, 2.0);

			mPhysicsWorld.dynamicsWorld()->setInternalTickCallback(CharacterCollidedTickCallback, this, true);
			player.characterController.setPhysicsWorld(&mPhysicsWorld);
			
			mGraphicsScene.sceneObjects2D = &player.characterHealthIndicator.healthIndicators();

			mGraphicsScene.setCamera(player.camera);

			mPlayer = &player;
			mPlayerInitPosition = mPlayer->character.transform().translation;

			mPlayer->character.physicsBody()->setUserIndex(LevelObjectIndex::ePlayer);

			for (auto& trap : mDelayedDamageTraps)
			{
				trap.get().setPlayer(mPlayer);
			}
			for (auto& trap : mWindTraps)
			{
				trap.get().setPlayer(mPlayer);
			}
			for (auto& trap : mTrampolineTraps)
			{
				trap->setPlayer(mPlayer);
			}
		}

		void setSkyBox(GraphicsEngine::SkyBox& skybox)
		{
			mGraphicsScene.setSkybox(skybox);
		}

		void killPlayer() {
			mPlayer->characterHealthIndicator.healthIndicators().clear();
			bPlayerDied = true;
		}

		void restart() {
			mPlayer->character.setPos(mPlayerInitPosition);
			mPlayer->character.transform().rotation.y = 0.0;

			for (auto& trap : mDisappearingTraps)
			{
				trap->reset();
			}

			mPhysicsWorld.updateSimulation();

			for (auto& trap : mDelayedDamageTraps)
			{
				trap.get().reset();
			}

			mPlayer->characterHealthIndicator.reset();

			mPlayer->camera.reset();

			bPlayerDied = bPlayerPassed = bTimerStarted = false;
		}

		void update(float dt)
		{
			if (mPlayer->character.transform().translation.y <= -2) killPlayer();

			if (bPlayerPassed || bPlayerDied || mPlayer->characterHealthIndicator.healthIndicators().empty()) return;

			mPlayer->characterController.update(dt);

			for (auto& trap : mSpinningTraps)
			{
				trap.get().spin(dt, 0.5f);
			}
			for (auto& trap : mWindTraps)
			{
				trap.get().update();				
			}

			for (int i = 0; i < mDisappearingTraps.size(); ++i)
			{
				auto& trap = mDisappearingTraps[i];
				trap->update();
			}

			mPhysicsWorld.updateSimulation();

			mPlayer->character.update();

			for (auto& trap : mDelayedDamageTraps)
			{
				trap.get().update();
			}

			mPlayer->camera.followObject(mPlayer->character.graphicsMesh());
			mPlayer->camera.update(dt);
		}
	
		Player* player() { return mPlayer; }

		void startTimer() {
			if (bTimerStarted) return;
			bTimerStarted = true;
			mGamePlayTimer.Restart();
		}

		float getGamePlayTime() const {
			return mGamePlayTimer.GetElapsedTime();
		}

		void playerPassed() { 
			bPlayerPassed = true;
		}
		
		bool isVictory() const { return bPlayerPassed; }
	public:
		enum LevelObjectIndex {
			ePlayer = 7997,
			eStartLine = 7998,
			eFinishLine = 7999,
		};

	public:
		GraphicsEngine::GraphicsScene& graphicsScene() { return mGraphicsScene; }
		std::vector<std::reference_wrapper<DelayedDamageTrap>>& delayedDamageTraps() { return mDelayedDamageTraps; }
		std::vector<std::reference_wrapper<WindTrap>>& windTraps() { return mWindTraps; }
		std::vector<DisappearingTrap*>& disappearingTraps() { return mDisappearingTraps; }
		std::vector<TrampolineTrap*>& trampolineTraps() { return mTrampolineTraps; }
		
		GameEngine::PhysicsWorld& physicsWorld() { return mPhysicsWorld; }

	private:
		GraphicsEngine::GraphicsScene mGraphicsScene;
		GameEngine::PhysicsWorld mPhysicsWorld;

		GraphicsEngine::Timer::TimeStep mGamePlayTimer;

		std::vector<std::reference_wrapper<DelayedDamageTrap>> mDelayedDamageTraps;
		std::vector<std::reference_wrapper<SpinningTrap>> mSpinningTraps;
		std::vector<std::reference_wrapper<WindTrap>> mWindTraps;
		std::vector<DisappearingTrap*> mDisappearingTraps;
		std::vector<TrampolineTrap*> mTrampolineTraps;
		
		Player* mPlayer = nullptr;
		DirectX::XMFLOAT3 mPlayerInitPosition;

		bool bPlayerPassed = false;
		bool bPlayerDied = false;
		bool bTimerStarted = false;
	};

	void CharacterCollidedTickCallback(btDynamicsWorld* dynamicsWorld, btScalar timeStep)
	{
		Level* scene = static_cast<Level*>(dynamicsWorld->getWorldUserInfo());
		
		int delayedDamageTrapsCount = scene->delayedDamageTraps().size();
		int windTrapsCount = scene->windTraps().size();
		int disappearingTrapsCount = scene->disappearingTraps().size();
		int trampolineTrapsCount = scene->trampolineTraps().size();

		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; ++i) {
			btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

			auto* objA = contactManifold->getBody0();
			auto* objB = contactManifold->getBody1();

			int idxA = objA->getUserIndex();
			int idxB = objB->getUserIndex();

			if (idxB == Level::LevelObjectIndex::ePlayer)
			{
				if (idxA >= 0 && idxA < delayedDamageTrapsCount) {

					int numContacts = contactManifold->getNumContacts();

					if (numContacts) {
						auto& trap = scene->delayedDamageTraps()[idxA].get();
						trap.activate();
					}
				}
				else if (idxA >= delayedDamageTrapsCount && idxA < delayedDamageTrapsCount + windTrapsCount) {

					int numContacts = contactManifold->getNumContacts();

					if (numContacts) {
						auto& trap = scene->windTraps()[idxA - delayedDamageTrapsCount].get();
						trap.activate();
					}
				}
				else if (idxA >= delayedDamageTrapsCount + windTrapsCount && idxA < delayedDamageTrapsCount + windTrapsCount + trampolineTrapsCount) {
					int numContacts = contactManifold->getNumContacts();
					if (numContacts) {
						auto& trap = scene->trampolineTraps()[idxA - delayedDamageTrapsCount - windTrapsCount];
						trap->activate();
					}
				}
				else if (idxA >= delayedDamageTrapsCount + windTrapsCount + trampolineTrapsCount && idxA < delayedDamageTrapsCount + windTrapsCount + trampolineTrapsCount + disappearingTrapsCount) {

					int numContacts = contactManifold->getNumContacts();

					if (numContacts) {
						auto& trap = scene->disappearingTraps()[idxA - delayedDamageTrapsCount - windTrapsCount - trampolineTrapsCount];
						trap->activate();
					}
				}
				else if (idxA == scene->LevelObjectIndex::eStartLine)
				{
					int numContacts = contactManifold->getNumContacts();

					if (numContacts)
					{
						scene->startTimer();
					}
				}
				else if (idxA == scene->LevelObjectIndex::eFinishLine)
				{
					int numContacts = contactManifold->getNumContacts();

					if (numContacts)
					{
						scene->playerPassed();
					}
				}
			}
		}
	}
}
