#include "../Headers/Scene/Components/StaticMeshComponent.h"
#include "../Headers/Graphics/Resources/DrawableMesh.h"
#include "../Headers/ResourceManager.h"
#include "../Headers/Input/Camera.h"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

StaticMeshComponent::StaticMeshComponent(ResourceManager* pRM, std::string ModelPath, float scale)
	:
	Pos({0.0, 0.0, 0.0})
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(ModelPath.c_str(),
		//aiProcess_Triangulate |
		//aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded //|
		//aiProcess_GenNormals |
		//aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr)
		throw std::exception(imp.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* mesh = pScene->mMeshes[i];
		Meshes.emplace_back(pRM, mesh, pScene->mMaterials[mesh->mMaterialIndex], ModelPath, scale);
	}

	CB = pRM->CreateConstBuffer(&Transformation, sizeof(Transformation), 0);

	pRM->PrepareForRtx(this);
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
	PosMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos));
	Transformation = DirectX::XMMatrixTranspose(PosMatrix * cam->GetView() * cam->GetProjection());
	CB->Update(&Transformation, sizeof(Transformation));
}

void StaticMeshComponent::SetPos(DirectX::XMFLOAT3 Pos)
{
	this->Pos = Pos;
	this->PosMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos));
}

Engine_API DirectX::XMFLOAT3 StaticMeshComponent::GetPos()
{
	return Pos;
}

DirectX::XMMATRIX& StaticMeshComponent::GetPosMatrix()
{
	return PosMatrix;
}

Engine_API DirectX::XMMATRIX StaticMeshComponent::GetTransform()
{
	return Transformation;
}
