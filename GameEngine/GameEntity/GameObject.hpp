#pragma once
#include <memory>
#include <btBulletDynamicsCommon.h>
#include <Scene/SceneObject/Object3D.h>

namespace GameEngine {
	namespace Colors {
		const DirectX::XMFLOAT4 WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
		const DirectX::XMFLOAT4 BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
		const DirectX::XMFLOAT4 CYAN = { 0.0f, 1.0f, 1.0f, 1.0f };
		const DirectX::XMFLOAT4 TANGERINE_ORANGE = { 1.0f, 0.3f, 0.0f, 1.0f };
		const DirectX::XMFLOAT4 FLUORESCENT_YELLOW = { 0.8f, 1.0f, 0.0f, 1.0f };
		const DirectX::XMFLOAT4 LAVENDER = { 0.84f, 0.7f, 0.95f, 1.0f };

		const DirectX::XMFLOAT4 YELLOW = { 249.0f / 255.0f, 254.0f / 255.0f, 20.0f / 255.0f, 1.0f };
		const DirectX::XMFLOAT4 FUCHSIA = { 243.0f / 255.0f, 77.0f / 255.0f, 250.0f / 255.0f, 1.0f };
		const DirectX::XMFLOAT4 DARK_BLUE = { 47.0f / 255.0f, 62.0f / 255.0f, 143.0f / 255.0f, 1.0f };
		const DirectX::XMFLOAT4 TURQUOISE = { 60.0f / 255.0f, 181.0f / 255.0f, 203.0f / 255.0f, 1.0f };
		const DirectX::XMFLOAT4 LIGHT_PURPLE = { 152.0f / 255.0f, 120.0f / 255.0f, 253.0f / 255.0f, 1.0f };
		const DirectX::XMFLOAT4 DARK_PURPLE = { 83.0f / 255.0f, 59.0f / 255.0f, 177.0f / 255.0f, 1.0f };
		const DirectX::XMFLOAT4 MINT = { 0.0f / 255.0f, 254.0f / 255.0f, 158.0f / 255.0f, 1.0f };

		const DirectX::XMFLOAT4 LIME_GREEN = { 0.25f, 0.9f, 0.25f, 1.0f };
	};

	class GameObject
	{
	public:
		struct BoundingBox {
			const float width = 0.0f;
			const float height = 0.0f;
			const float depth = 0.0f;
		};
	public:
		GameObject(float width, float height, float depth, const DirectX::XMFLOAT4& color) 
			: mWidth(width), mHeight(height), mDepth(depth)
		{
			mGraphicsObject.InitializeAsBox(mWidth, mHeight, mDepth, color);
		}

		GameObject(const std::string& objPath, const BoundingBox& objModelBoundingBox)
			: mWidth(objModelBoundingBox.width), mHeight(objModelBoundingBox.height), mDepth(objModelBoundingBox.depth)
		{
			mGraphicsObject.InitializeAsObjModel(objPath.c_str());
		}

		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = default;

		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = default;
		
		~GameObject() = default;

	public:
		void update()
		{
			//btTransform updatedTransform;
			//mPhysicsBody->getMotionState()->getWorldTransform(updatedTransform);
			
			auto updatedTransform = mPhysicsBody->getWorldTransform();
			
			auto newPos = updatedTransform.getOrigin();
			mGraphicsObject.transform.translation = { static_cast<float>(newPos.x()), static_cast<float>(newPos.y()), static_cast<float>(newPos.z()) };
		}

		void setPos(const DirectX::XMFLOAT3& position) {
			mGraphicsObject.transform.translation = position;

			btTransform transform; 
			mPhysicsBody->getMotionState()->getWorldTransform(transform);
			transform.setOrigin({ position.x, position.y, position.z });
			mPhysicsBody->getMotionState()->setWorldTransform(transform);

			auto t = mPhysicsBody->getWorldTransform();
			t.setOrigin({ position.x, position.y, position.z });
			mPhysicsBody->setWorldTransform(t);
		}

	public:
		std::unique_ptr<btRigidBody>& physicsBody() { return mPhysicsBody; }
		GraphicsEngine::Object3D& graphicsMesh() { return mGraphicsObject; }

		float width() const { return mWidth; }
		float height() const { return mHeight; }
		float depth() const { return mDepth; }

		DirectX::XMFLOAT4& color() { return mGraphicsObject.color; }

		GraphicsEngine::TransformComponent& transform() { return mGraphicsObject.transform; }

	private:
		GraphicsEngine::Object3D mGraphicsObject;
		std::unique_ptr<btRigidBody> mPhysicsBody = nullptr;

		const float mWidth = 0.0f;
		const float mHeight = 0.0f;
		const float mDepth = 0.0f;
	};
}
