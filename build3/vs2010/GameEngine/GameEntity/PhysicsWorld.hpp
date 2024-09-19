#pragma once
#include "GameObject.hpp"

namespace GameEngine {
	class PhysicsWorld
	{
	public:
		enum PhysicsObjectType {
			eStatic,
			eDynamic,
			eKinematic
		};
	public:
		void addPhysicsObject(GameObject& obj, PhysicsObjectType type)
		{
			mDynamicsWorld->setGravity(btVector3(0, -10, 0));

			mCollisionShapes.emplace_back(std::make_unique<btBoxShape>(btVector3(obj.width() / 2.0f, obj.height() / 2.0f, obj.depth() / 2.0f)));

			auto position = obj.transform().translation;

			btTransform objTransform;
			objTransform.setIdentity();
			objTransform.setOrigin(btVector3(position.x, position.y, position.z));

			btScalar mass(0.);
			btVector3 localInertia(0, 0, 0);

			switch (type) {
			case PhysicsObjectType::eStatic: {
				mMotionStates.emplace_back(std::make_unique<btDefaultMotionState>(objTransform));
				break;
			}
			case PhysicsObjectType::eDynamic: {
				mass = 3.0f;
				mCollisionShapes.back()->calculateLocalInertia(mass, localInertia);
				mMotionStates.emplace_back(std::make_unique<btDefaultMotionState>(objTransform));
				break;
			}
			case PhysicsObjectType::eKinematic: {
				mMotionStates.emplace_back(std::make_unique<btDefaultMotionState>(objTransform));
				break;
			}
			}
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionStates.back().get(), mCollisionShapes.back().get(), localInertia);
			obj.physicsBody() = std::make_unique<btRigidBody>(rbInfo);
			mDynamicsWorld->addRigidBody(obj.physicsBody().get());
		}
		
		void updateSimulation() { mDynamicsWorld->stepSimulation(1 / 60.0f, 5); }
		
		//std::unique_ptr<btDiscreteDynamicsWorld>& dynamicsWorld() { return mDynamicsWorld; }
		btDiscreteDynamicsWorld* dynamicsWorld() { return mDynamicsWorld; }

		std::vector<std::unique_ptr<btMotionState>>& motionStates() { return mMotionStates; }
		std::vector<std::unique_ptr<btCollisionShape>>& collisionShapes() { return mCollisionShapes; }

	private:
		std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
		std::unique_ptr<btCollisionDispatcher> mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());
		std::unique_ptr<btBroadphaseInterface> mOverlappingPairCache = std::make_unique<btDbvtBroadphase>();
		std::unique_ptr<btSequentialImpulseConstraintSolver> mSolver = std::make_unique<btSequentialImpulseConstraintSolver>();

		//std::unique_ptr<btDiscreteDynamicsWorld> mDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(mDispatcher.get(), mOverlappingPairCache.get(), mSolver.get(), mCollisionConfiguration.get());
		btDiscreteDynamicsWorld* mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher.get(), mOverlappingPairCache.get(), mSolver.get(), mCollisionConfiguration.get());
		std::vector<std::unique_ptr<btCollisionShape>> mCollisionShapes;
		std::vector<std::unique_ptr<btMotionState>> mMotionStates;
	};
}
