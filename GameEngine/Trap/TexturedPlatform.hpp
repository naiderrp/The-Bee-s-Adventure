#pragma once
#include <GameEngine/GameEntity/GameObject.hpp>
#include <GameEngine/GameEntity/PhysicsWorld.hpp>

#include <Scene/SceneObject/ScreenText.h>

namespace GameEngine {
	class TexturedPlatform {
	public:
		TexturedPlatform(float width, float height, float depth, float textureZpos, float textureDepth, const DirectX::XMFLOAT4& color)
			: mPlatform(width, height, depth, color)
		{
			mTextureWidth = width;
			mTextureDepth = textureDepth;

			mLineTexture.InitializeAsTexturedRectangle(mTextureWidth, 0.0, 1.0, "textures/");
			
			mLineTexture.transform.translation.y = height / 2.0 + 0.05;
			mLineTexture.transform.translation.z = textureZpos;
		}

		void translate(const DirectX::XMFLOAT3& translation) {
			mPlatform.transform().translation.x += translation.x;
			mPlatform.transform().translation.y += translation.y;
			mPlatform.transform().translation.z += translation.z;

			mLineTexture.transform.translation.x += translation.x;
			mLineTexture.transform.translation.y += translation.y;
			mLineTexture.transform.translation.z += translation.z;
		}

		GameEngine::GameObject& gameObject() { return mPlatform; }
		GraphicsEngine::Object3D& textureGraphicsObject() { return mLineTexture; }

		void initLinePhysicsBody(GameEngine::PhysicsWorld& physicsWorld, float offset = 0.0) {
			physicsWorld.collisionShapes().emplace_back(std::make_unique<btBoxShape>(btVector3(mTextureWidth / 2.0f, 0.0, (mTextureDepth + offset) / 2.0f)));

			btTransform objTransform;
			objTransform.setIdentity();

			float zPos = mLineTexture.transform.translation.z + mTextureDepth + offset;
			
			objTransform.setOrigin(btVector3(mLineTexture.transform.translation.x, mLineTexture.transform.translation.y, zPos));

			btScalar mass(0.);
			btVector3 localInertia(0, 0, 0);

			physicsWorld.motionStates().emplace_back(std::make_unique<btDefaultMotionState>(objTransform));

			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, physicsWorld.motionStates().back().get(), physicsWorld.collisionShapes().back().get(), localInertia);
			mLinePhysicsBody = std::make_unique<btRigidBody>(rbInfo);
			physicsWorld.dynamicsWorld()->addRigidBody(mLinePhysicsBody.get());
		}

		btRigidBody* linePhysicsBody() { return mLinePhysicsBody.get(); }

	private:
		GameEngine::GameObject mPlatform;
		GraphicsEngine::Object3D mLineTexture;
		std::unique_ptr<btRigidBody> mLinePhysicsBody;
		float mTextureWidth;
		float mTextureDepth;
	};
}     