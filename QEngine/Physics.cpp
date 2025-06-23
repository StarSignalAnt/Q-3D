#include "Physics.h"
#include <iostream>

static PxDefaultAllocator      gAllocator;
static PxDefaultErrorCallback  gErrorCallback;

Physics::Physics() {

    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    if (!gFoundation)
    {
        std::cout << "Failed to create physics foundation.\n";
        // handle error
    }

    bool recordMemoryAllocations = true;
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), recordMemoryAllocations, nullptr);
    if (!gPhysics)
    {
        std::cout << "Failed to create physics.\n";
        // handle error
    }

    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    if (!gDispatcher)
        throw std::runtime_error("PxDefaultCpuDispatcherCreate failed!");


    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);  // Standard gravity
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    gScene = gPhysics->createScene(sceneDesc);
    if (!gScene)
        throw std::runtime_error("createScene failed!");

    physx::PxTolerancesScale scale; // Typically created for PxPhysics

    physx::PxCookingParams cookingParams(scale);
    // Optional settings:
    cookingParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    //cookingParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES;

    m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, cookingParams);

    if (!m_Cooking)
        throw std::runtime_error("Failed to create PxCooking!");

}

PxGeometry Physics::CreateBox(glm::vec3 size) {

    PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // static friction, dynamic friction, restitution

    // 2. Define box geometry (half extents in meters)
    PxBoxGeometry boxGeometry(PxVec3(size.x,size.y,size.z)); // 2x2x2 meter cube

    return boxGeometry;

}

PxRigidDynamic* Physics::CreateRB() {

    PxTransform transform( PxVec3(0.0f, 5.0f, 0.0f));
    PxRigidDynamic* rigidBody = gPhysics->createRigidDynamic(transform);
    PxRigidBodyExt::updateMassAndInertia(*rigidBody, 10.0f);
    return rigidBody;

}