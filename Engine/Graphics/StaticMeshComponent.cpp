#include "../Headers/Scene/Components/StaticMeshComponent.h"
#include "../Headers/Graphics/Resources/DrawableMesh.h"
#include "../Headers/ResourceManager.h"
#include "../Headers/Input/Camera.h"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

StaticMeshComponent::StaticMeshComponent(ResourceManager* pRM, std::string ModelPath, float scale)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(ModelPath.c_str(),
		//aiProcess_Triangulate |
		//aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr)
		throw std::exception(imp.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* mesh = pScene->mMeshes[i];
		Meshes.emplace_back(pRM, mesh, pScene->mMaterials[mesh->mMaterialIndex], ModelPath, scale);
	}

	Transform.PosMatrix = DirectX::XMMatrixIdentity();

	CB = pRM->CreateConstBuffer(&DxTransform, sizeof(DxTransform), 0);
}

void StaticMeshComponent::Draw(Graphics* pGraphics)
{
	for (auto& m : Meshes)
	{
		m.BindMaterial(pGraphics);
		CB->Bind(pGraphics);
		m.Draw(pGraphics);
	}
}

void StaticMeshComponent::Update(Camera* cam)
{
	DxTransform.Pos = DirectX::XMMatrixTranspose(Transform.PosMatrix);
	DxTransform.PosViewProj = DirectX::XMMatrixTranspose(Transform.PosMatrix * cam->GetView() * cam->GetProjection());
	DxTransform.View = DirectX::XMMatrixTranspose(cam->GetView());
	DxTransform.Proj = DirectX::XMMatrixTranspose(cam->GetProjection());
	DxTransform.ViewPos = cam->GetPos();

	CB->Update(&DxTransform, sizeof(DxTransform));
}

void StaticMeshComponent::SetPos(DirectX::XMFLOAT3 Pos)
{
	Transform.Pos = Pos;
	Transform.PosMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * DirectX::XMMatrixRotationQuaternion(Transform.RotQuat);
}

void StaticMeshComponent::SetRotation(DirectX::XMFLOAT3 Rotation)
{
	Transform.Rotation = Rotation;
	Transform.RotQuat = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&Rotation));
	Transform.PosMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Transform.Pos)) * DirectX::XMMatrixRotationQuaternion(Transform.RotQuat);
}

Engine_API DirectX::XMFLOAT3 StaticMeshComponent::GetPos()
{
	return Transform.Pos;
}

DirectX::XMFLOAT3 StaticMeshComponent::GetRotation()
{
	return Transform.Rotation;
}

DirectX::XMMATRIX& StaticMeshComponent::GetPosMatrix()
{
	return Transform.PosMatrix;
}

Engine_API DirectX::XMMATRIX StaticMeshComponent::GetTransform()
{
	return {};
}
