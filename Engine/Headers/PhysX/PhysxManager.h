#pragma once
#include <PxPhysicsAPI.h>
#include <DirectXMath.h>
#include "../Header.h"

class PhysxManager
{
public:
	Engine_API PhysxManager();
	Engine_API ~PhysxManager();

	//physx::PxScene* CreatePhysxScene();

	Engine_API void AddToScene(physx::PxActor* pActor);

	Engine_API physx::PxRigidStatic* CreateRigidStatic(DirectX::XMMATRIX transform);
	Engine_API physx::PxRigidDynamic* CreateRigidDynamic(DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation = { 0, 0, 0 });

	Engine_API physx::PxMaterial* CreateMaterial(float sf = 0.5f, float df = 0.5f, float r = 0.1f);    //static friction, dynamic friction, restitution);

	Engine_API void Simulate(float dt);

private:
	physx::PxFoundation* pFoundation;
	physx::PxScene* pScene;

	physx::PxTolerancesScale Scale;
	physx::PxPhysics* pPhysics;
	physx::PxCooking* pCooking;

	physx::PxDefaultErrorCallback DefaultErrorCallback;
	physx::PxDefaultAllocator DefaultAllocatorCallback;

	physx::PxDefaultCpuDispatcher* pDefaultCpuDispatcher;
	physx::PxSimulationFilterShader* pDefaultSimulationFilterShader;

	physx::PxPvd* pPvd;
	physx::PxPvdTransport* pPvdTransport;
};