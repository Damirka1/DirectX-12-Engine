#include "Sphere.h"
#include "../../Headers/Graphics/Resources/DrawableMesh.h"
#include "../../Headers/ResourceManager.h"
#include "../../Headers/Input/Camera.h"
#include <random>

Sphere::Sphere(ResourceManager* pRM, PhysxManager* pPM, DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation)
	:
	StaticMeshComponent(pRM, "C:\\Home\\GraphicsProjects\\3d models\\Sphere\\Sphere.obj")
{
	StaticMeshComponent::SetPos(Pos);
	StaticMeshComponent::SetRotation(Rotation);

	pRigidDynamic = pPM->CreateRigidDynamic(StaticMeshComponent::GetPos());
	pMaterial = pPM->CreateMaterial(0.2f, 0.8f, 0.0f);

	pShape = physx::PxRigidActorExt::createExclusiveShape(*pRigidDynamic,
		physx::PxSphereGeometry(1.0f), *pMaterial);

	pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

	pPM->AddToScene(pRigidDynamic);

	static std::default_random_engine e;
	static std::uniform_real_distribution<float> dis(0, 1); // range [0, 1)
	
	for (auto& m : Meshes)
	{
		DirectX::XMFLOAT3 color = { dis(e), dis(e), dis(e) };
		m.UpdateColor(color);
	}

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
