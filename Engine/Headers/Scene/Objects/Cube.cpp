#include "Cube.h"
#include "../../Headers/Graphics/Resources/DrawableMesh.h"
#include "../../Headers/ResourceManager.h"
#include "../../Headers/Input/Camera.h"

Cube::Cube(ResourceManager* pRM, PhysxManager* pPM)
	:
	StaticMeshComponent(pRM, "C:\\Home\\GraphicsProjects\\3d models\\Cube\\Cube.obj")
{
	StaticMeshComponent::SetPos({ 0.0f, 13.0f, 0.0f });

	pRigidDynamic = pPM->CreateRigidDynamic(StaticMeshComponent::GetPos());
	pMaterial = pPM->CreateMaterial(0.2f, 0.8f, 0.1f);

	pShape = physx::PxRigidActorExt::createExclusiveShape(*pRigidDynamic,
		physx::PxBoxGeometry(1.0f, 1.0f, 1.0f), *pMaterial);

	pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

	pPM->AddToScene(pRigidDynamic);
}

void Cube::UpdateBody(Camera* cam)
{
	physx::PxMat44 tf = pRigidDynamic->getGlobalPose();

	DirectX::XMMATRIX m(tf.front());

	Transformation = DirectX::XMMatrixTranspose(m * cam->GetView() * cam->GetProjection());
	CB->Update(&Transformation, sizeof(Transformation));
}

Engine_API void Cube::AddForce(DirectX::XMFLOAT3 vec)
{
	physx::PxVec3 f;
	f.x = vec.x;
	f.y = vec.y;
	f.z = vec.z;

	pRigidDynamic->addForce(f);
}
