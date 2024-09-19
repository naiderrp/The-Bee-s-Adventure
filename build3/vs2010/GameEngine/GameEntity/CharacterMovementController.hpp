#pragma once
#include <btBulletDynamicsCommon.h>
#include <Scene/SceneObject/Object3D.h>
#include <Scene/Camera.h>

#include "GameObject.hpp"
#include "PhysicsWorld.hpp"

namespace GameEngine {
	class ClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback {
	public:
		ClosestNotMeRayResultCallback(btRigidBody* me, const btVector3& rayFromWorld, const btVector3& rayToWorld)
			: btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld), mMe(me) {}

		btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) override {
			if (rayResult.m_collisionObject == mMe) {
				return 1.0f;
			}
			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}

	protected:
		btRigidBody* mMe = nullptr;
	};

	class CharacterMovementController {
		enum KeyMappings {
			eTurnRight = 'D',
			eTurnLeft = 'A',
			eMoveForward = 'W',
			eMoveBackward = 'S',
			eJump = VK_SPACE
		};

	public:
		CharacterMovementController(GameEngine::GameObject& character, GraphicsEngine::Camera& camera)
		: mCharacter(character), mCamera(camera), mRayHitCallback(mCharacter.physicsBody().get(), {}, {})
		{
		}

		void update(float dt) {
			resetVelocity();

			if (GetAsyncKeyState(KeyMappings::eTurnLeft)) {
				processTurnLeft(dt);
			}
			if (GetAsyncKeyState(KeyMappings::eTurnRight)) {
				processTurnRight(dt);
			}

			if (GetAsyncKeyState(KeyMappings::eMoveForward)) {
				processMoveForward(dt);
			}

			if (GetAsyncKeyState(KeyMappings::eMoveBackward)) {
				processMoveBackward(dt);
			}

			if (GetAsyncKeyState(KeyMappings::eJump)) {
				processJump();
			}
		}

		void setPhysicsWorld(PhysicsWorld* physicsWorld) { mPhysicsWorld = physicsWorld; }

	private:
		bool isGrounded() {
			// Reset out callback 
			{
				mRayHitCallback.m_closestHitFraction = 1.0f;
				mRayHitCallback.m_collisionObject = nullptr;
			}
			auto position = getPosition(mCharacter.graphicsMesh());
			auto tmpPosition = position;
			tmpPosition.y -= 1.4f;
			tmpPosition.z -= mCharacter.width() / 2.0;

			mPhysicsWorld->dynamicsWorld()->rayTest(
				{ position.x, position.y, position.z },
				{ tmpPosition.x, tmpPosition.y, tmpPosition.z },
				mRayHitCallback
			);

			bool backFaceOnGround = mRayHitCallback.hasHit();

			tmpPosition.z += mCharacter.width();

			mPhysicsWorld->dynamicsWorld()->rayTest(
				{ position.x, position.y, position.z },
				{ tmpPosition.x, tmpPosition.y, tmpPosition.z },
				mRayHitCallback
			);

			bool frontFaceOnGround = mRayHitCallback.hasHit();

			return backFaceOnGround || frontFaceOnGround;
		}

	private:
		void resetVelocity() { mLinearVelocity = { 0.0, 0.0, 0.0 }; }

		void rotateCharacter(float rotate, float dt) { mCharacter.transform().rotation.y += rotate * dt * mTurnSpeed; }

		void moveCharacter(float dt) {
			float yaw = mCharacter.transform().rotation.y;
			DirectX::XMFLOAT3 forwardDir = { sin(yaw), 0.f, cos(yaw) };

			DirectX::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };

			moveDir.x += forwardDir.x;
			moveDir.y += forwardDir.y;
			moveDir.z += forwardDir.z;

			auto normalizedDir = normalizeVector(moveDir);
			mLinearVelocity = { normalizedDir.x * dt * mMoveSpeed, normalizedDir.y * dt * mMoveSpeed, normalizedDir.z * dt * mMoveSpeed };
			mCharacter.physicsBody()->applyCentralImpulse(mLinearVelocity);
		}

	private:
		void processTurnLeft(float dt) {
			float rotate = 0.0f;

			if (mFlipped) rotate += 1.0f;
			else rotate -= 1.0f;

			if (mFlipped && mCharacter.transform().rotation.y < mCamera.mYaw - DirectX::XM_PIDIV2) {
				rotateCharacter(rotate, dt);
			}
			else if (!mFlipped && mCharacter.transform().rotation.y > mCamera.mYaw - DirectX::XM_PIDIV2) {
				rotateCharacter(rotate, dt);
			}
			else {
				moveCharacter(dt);
			}
		}

		void processTurnRight(float dt) {
			float rotate = 0.0f;

			if (mFlipped) rotate -= 1.0f;
			else rotate += 1.0f;

			if (mFlipped && mCharacter.transform().rotation.y > mCamera.mYaw - DirectX::XM_PI - DirectX::XM_PIDIV2) {
				rotateCharacter(rotate, dt);
			}
			else if (!mFlipped && mCharacter.transform().rotation.y < mCamera.mYaw + DirectX::XM_PIDIV2) {
				rotateCharacter(rotate, dt);
			}
			else {
				moveCharacter(dt);
			}
		}

		void processMoveForward(float dt) {
			mFlipped = false;
			mCharacter.transform().rotation.y = mCamera.mYaw;

			float yaw = mCharacter.transform().rotation.y;
			DirectX::XMFLOAT3 forwardDir = { sin(yaw), 0.0, cos(yaw) };
			DirectX::XMFLOAT3 rightDir = { forwardDir.z, 0.f, -forwardDir.x };
			const DirectX::XMFLOAT3 upDir = { 0.f, 1.f, 0.f };

			DirectX::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };

			if (GetAsyncKeyState(KeyMappings::eTurnLeft)) {

				mCharacter.transform().rotation = { 0.0, mCamera.mYaw - DirectX::XM_PIDIV4, 0.0 };

				forwardDir.x -= rightDir.x;
				forwardDir.y -= rightDir.y;
				forwardDir.z -= rightDir.z;
			}
			if (GetAsyncKeyState(KeyMappings::eTurnRight)) {

				mCharacter.transform().rotation = { 0.0, mCamera.mYaw + DirectX::XM_PIDIV4, 0.0 };

				forwardDir.x += rightDir.x;
				forwardDir.y += rightDir.y;
				forwardDir.z += rightDir.z;
			}

			moveDir.x += forwardDir.x;
			moveDir.y += forwardDir.y;
			moveDir.z += forwardDir.z;

			auto normalizedDir = normalizeVector(moveDir);
			mLinearVelocity = { normalizedDir.x * dt * mMoveSpeed, normalizedDir.y * dt * mMoveSpeed, normalizedDir.z * dt * mMoveSpeed };
			mCharacter.physicsBody()->applyCentralImpulse(mLinearVelocity);
		}

		void processMoveBackward(float dt) {
			mFlipped = true;
			mCharacter.transform().rotation.y = mCamera.mYaw - DirectX::XM_PI;

			float yaw = mCharacter.transform().rotation.y;
			DirectX::XMFLOAT3 forwardDir = { sin(yaw), 0.f, cos(yaw) };
			DirectX::XMFLOAT3 rightDir = { forwardDir.z, 0.f, -forwardDir.x };
			const DirectX::XMFLOAT3 upDir = { 0.f, 1.f, 0.f };

			DirectX::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };

			if (GetAsyncKeyState(KeyMappings::eTurnLeft)) {

				mCharacter.transform().rotation = { 0.0, mCamera.mYaw - DirectX::XM_PI + DirectX::XM_PIDIV4, 0.0 };

				forwardDir.x += rightDir.x;
				forwardDir.y += rightDir.y;
				forwardDir.z += rightDir.z;
			}
			if (GetAsyncKeyState(KeyMappings::eTurnRight)) {

				mCharacter.transform().rotation = { 0.0, mCamera.mYaw - DirectX::XM_PI - DirectX::XM_PIDIV4, 0.0 };

				forwardDir.x -= rightDir.x;
				forwardDir.y -= rightDir.y;
				forwardDir.z -= rightDir.z;
			}

			moveDir.x += forwardDir.x;
			moveDir.y += forwardDir.y;
			moveDir.z += forwardDir.z;

			auto normalizedDir = normalizeVector(moveDir);
			mLinearVelocity = { normalizedDir.x * dt * mMoveSpeed, normalizedDir.y * dt * mMoveSpeed, normalizedDir.z * dt * mMoveSpeed };
			mCharacter.physicsBody()->applyCentralImpulse(mLinearVelocity);
		}

		void processJump() {
			if (!isGrounded()) return;

			mLinearVelocity.setY(mLinearVelocity.y() + mJumpFactor);
			mCharacter.physicsBody()->applyCentralImpulse(mLinearVelocity);
		}

	private:
		DirectX::XMFLOAT3 normalizeVector(const DirectX::XMFLOAT3& vector) {
			DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&vector);

			vec = DirectX::XMVector3Normalize(vec);

			DirectX::XMFLOAT3 normalizedVector;
			DirectX::XMStoreFloat3(&normalizedVector, vec);

			return normalizedVector;
		}

		DirectX::XMFLOAT3& getPosition(GraphicsEngine::Object3D& object) { return object.transform.translation; }

	private:
		GameEngine::GameObject& mCharacter;
		GameEngine::PhysicsWorld* mPhysicsWorld = nullptr;

		GraphicsEngine::Camera& mCamera;
		
		btVector3 mLinearVelocity = { 0.0, 0.0, 0.0 };

		const float mMoveSpeed = 50.0;
		const float mTurnSpeed = 7.0f;
		const double mJumpFactor = 8;
		
		bool mFlipped = false;

		ClosestNotMeRayResultCallback mRayHitCallback;
	};
}
