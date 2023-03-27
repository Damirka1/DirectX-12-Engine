#include "../Headers/PhysX/PhysxManager.h"
#include <exception>

PhysxManager::PhysxManager()
{
    pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, DefaultAllocatorCallback,
        DefaultErrorCallback);
    if (!pFoundation)
        throw std::exception("Can't init PhysX");

    pPvd = physx::PxCreatePvd(*pFoundation);
    pPvdTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    pPvd->connect(*pPvdTransport, physx::PxPvdInstrumentationFlag::eALL);

    Scale = physx::PxTolerancesScale::PxTolerancesScale();

    pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pFoundation,
        Scale, false, pPvd);
    if (!pPhysics)
        throw std::exception("Can't init PhysX");

    pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *pFoundation, physx::PxCookingParams(Scale));
    if (!pCooking)
        throw std::exception("Can't init PhysX");

    // create CPU dispatcher which mNbThreads worker threads
    pDefaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    if (!pDefaultCpuDispatcher)
        throw std::exception("Can't init PhysX");

    physx::PxSceneDesc DefaultSceneDesc = physx::PxSceneDesc(Scale);
    DefaultSceneDesc.cpuDispatcher = pDefaultCpuDispatcher;
    DefaultSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    DefaultSceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);


    pScene = pPhysics->createScene(DefaultSceneDesc);
}

PhysxManager::~PhysxManager()
{
    pCooking->release();
    pPhysics->release();
    pFoundation->release();

}


void PhysxManager::AddToScene(physx::PxActor* pActor)
{
    pScene->addActor(*pActor);
}

physx::PxRigidStatic* PhysxManager::CreateRigidStatic(DirectX::XMFLOAT3 Pos)
{
    return pPhysics->createRigidStatic(physx::PxTransform(Pos.x, Pos.y, Pos.z));
}

physx::PxRigidDynamic* PhysxManager::CreateRigidDynamic(DirectX::XMFLOAT3 Pos)
{
    return pPhysics->createRigidDynamic(physx::PxTransform(Pos.x, Pos.y, Pos.z));
}

physx::PxMaterial* PhysxManager::CreateMaterial(float sf, float df, float r)
{
    physx::PxMaterial* pMaterial = pPhysics->createMaterial(sf, df, r);
    return pMaterial;
}

void PhysxManager::Simulate(float dt)
{
    pScene->simulate(dt);
    pScene->fetchResults(true);
}
