#include "Sphere.h"
#include "../../Headers/Graphics/Resources/DrawableMesh.h"
#include "../../Headers/ResourceManager.h"
#include "../../Headers/Input/Camera.h"

Sphere::Sphere(ResourceManager* pRM, PhysxManager* pPM)
	:
	StaticMeshComponent(pRM, "C:\\Home\\GraphicsProjects\\3d models\\Sphere\\Sphere.obj")
{
	StaticMeshComponent::SetPos({ 0.0f, 10.0f, 0.0f });

	pRigidDynamic = pPM->CreateRigidDynamic(StaticMeshComponent::GetPos());
	pMaterial = pPM->CreateMaterial(0.2f, 0.8f, 0.0f);

	pShape = physx::PxRigidActorExt::createExclusiveShape(*pRigidDynamic,
		physx::PxSphereGeometry(1.0f), *pMaterial);

	pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

	pPM->AddToScene(pRigidDynamic);

	pRM->PrepareForRtx(this, 0);
}

void Sphere::UpdateBody(Camera* cam)
{
	physx::PxMat44 tf = pRigidDynamic->getGlobalPose();

	DirectX::XMMATRIX m(tf.front());

	Transform.PosMatrix = m;
	DxTransform.Pos = DirectX::XMMatrixTranspose(Transform.PosMatrix);
	DxTransform.PosViewProj = DirectX::XMMatrixTranspose(Transform.PosMatrix * cam->GetView() * cam->GetProjection());

	CB->Update(&DxTransform, sizeof(DxTransform));
}

Engine_API void Sphere::AddForce(DirectX::XMFLOAT3 vec)
{
	physx::PxVec3 f;
	f.x = vec.x;
	f.y = vec.y;
	f.z = vec.z;

	pRigidDynamic->addForce(f);
}
