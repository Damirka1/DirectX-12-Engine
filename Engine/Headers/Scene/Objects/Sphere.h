#pragma once
#include "../Components/StaticMeshComponent.h"
#include "../../PhysX/PhysxManager.h"

class Sphere : public StaticMeshComponent
{
public:
	Engine_API Sphere(SceneResources* pSceneResources, DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation);

	Engine_API void Update(Camera* cam) override;

	Engine_API void AddForce(DirectX::XMFLOAT3 vec);

private:
	physx::PxRigidDynamic* pRigidDynamic;
	physx::PxShape* pShape;
	physx::PxMaterial* pMaterial;
};