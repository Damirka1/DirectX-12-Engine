#include "Cube.h"
#include "../../Headers/Graphics/Resources/DrawableMesh.h"
#include "../../Headers/ResourceManager.h"
#include "../../Headers/Scene/SceneResources.h"
#include "../../Headers/Input/Camera.h"
#include <random>

Cube::Cube(SceneResources* pSceneResources, DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation)
	:
	StaticMeshComponent(pSceneResources, "C:\\Home\\GraphicsProjects\\3d models\\Cube\\Cube.obj")
{
	StaticMeshComponent::SetPos(Pos);
	StaticMeshComponent::SetRotation(Rotation);

	auto pPM = pSceneResources->pPhysx;

	pRigidDynamic = pPM->CreateRigidDynamic(StaticMeshComponent::GetPos());
	pMaterial = pPM->CreateMaterial(0.2f, 0.8f, 0.1f);

	pShape = physx::PxRigidActorExt::createExclusiveShape(*pRigidDynamic,
		physx::PxBoxGeometry(1.0f, 1.0f, 1.0f), *pMaterial);

	pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

	pPM->AddToScene(pRigidDynamic);

	static std::default_random_engine e;
	static std::uniform_real_distribution<float> dis(0, 1); // range [0, 1)

	for (auto& m : Meshes)
	{
		DirectX::XMFLOAT3 color = { dis(e), dis(e), dis(e) };
		m.UpdateColor(color);
	}

	//pRM->PrepareForRtx(this, 0);
}

void Cube::Update(Camera* cam)
{
	physx::PxMat44 tf = pRigidDynamic->getGlobalPose();

	DirectX::XMMATRIX m(tf.front());

	Transform.PosMatrix = m;
	//DxTransform.Pos = DirectX::XMMatrixTranspose(Transform.PosMatrix);
	//DxTransform.PosViewProj = DirectX::XMMatrixTranspose(Transform.PosMatrix * cam->GetView() * cam->GetProjection());
	
	//CB->Update(&DxTransform, sizeof(DxTransform));

	StaticMeshComponent::Update(cam);
}

Engine_API void Cube::AddForce(DirectX::XMFLOAT3 vec)
{
	physx::PxVec3 f;
	f.x = vec.x;
	f.y = vec.y;
	f.z = vec.z;

	pRigidDynamic->addForce(f);
}
