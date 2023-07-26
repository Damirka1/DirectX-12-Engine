#pragma once
#include "../Components/StaticMeshComponent.h"
#include "../../PhysX/PhysxManager.h"

class Plane : public StaticMeshComponent
{
public:
	Plane(ResourceManager* pRM, PhysxManager* pPM, DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation)
		:
		StaticMeshComponent(pRM, "C:\\Home\\GraphicsProjects\\3d models\\Plane\\Plane.obj", 20)
	{
		StaticMeshComponent::SetPos(Pos);
		StaticMeshComponent::SetRotation(Rotation);
		//DirectX::XMFLOAT3 rot = { 45.0f, 0.0f, 0.0f };
		pRigidStatic = pPM->CreateRigidStatic(StaticMeshComponent::GetPosMatrix());
		pMaterial = pPM->CreateMaterial();

		pShape = physx::PxRigidActorExt::createExclusiveShape(*pRigidStatic,
			physx::PxBoxGeometry(20, 0.01, 20), *pMaterial);


		pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

		pPM->AddToScene(pRigidStatic);

		pRM->PrepareForRtx(this, 0);
	}

private:
	physx::PxRigidStatic* pRigidStatic;
	physx::PxShape* pShape;
	physx::PxMaterial* pMaterial;
};