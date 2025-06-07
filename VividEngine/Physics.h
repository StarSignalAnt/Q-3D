#pragma once
#include <glm/glm.hpp>
#include "PxPhysicsAPI.h"

using namespace physx;

class Physics
{
public:

	Physics();
	PxGeometry CreateBox(glm::vec3 size);
	PxRigidDynamic* CreateRB();
	PxMaterial* CreateMaterial() {
		return	gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // static friction, dynamic friction, restitution
	}
	void AddActor(PxRigidDynamic* rb)
	{
		gScene->addActor(*rb);
	}
	void RemoveActor(PxRigidDynamic* rb) {
		gScene->removeActor(*rb);
				// Now delete the actor if you're done with it:
		rb->release();
	}

	void RemoveStatic(PxRigidStatic* rs) {
		gScene->removeActor(*rs);
		rs->release();
	}

	void AddStatic(PxRigidStatic* stat)
	{
		gScene->addActor(*stat);
	}
	void Update(float dt) {
		gScene->simulate(dt);      // Advance the simulation
		gScene->fetchResults(true);
	}
	physx::PxCooking* GetCooking() const { return m_Cooking; }
	PxPhysics* GetPhysics() {
		return gPhysics;
	}
private:

	PxFoundation* gFoundation = nullptr;
	PxPhysics* gPhysics = nullptr;
	PxDefaultCpuDispatcher* gDispatcher = nullptr;
	PxScene* gScene = nullptr;
	PxCooking* m_Cooking = nullptr;
};

