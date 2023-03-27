#pragma once
#include "../Components/StaticMeshComponent.h"
#include "../../PhysX/PhysxManager.h"

class Cube : public StaticMeshComponent
{
public:
	Engine_API Cube(ResourceManager* pRM, PhysxManager* pPM);

	Engine_API void UpdateBody(Camera* cam);

	Engine_API void AddForce(DirectX::XMFLOAT3 vec);

private:
	physx::PxRigidDynamic* pRigidDynamic;
	physx::PxShape* pShape;
	physx::PxMaterial* pMaterial;
};