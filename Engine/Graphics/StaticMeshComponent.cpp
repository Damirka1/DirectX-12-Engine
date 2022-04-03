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
		aiProcess_Triangulate |
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
		Meshes.emplace_back(pRM, mesh, nullptr, scale);
	}

	CB = pRM->CreateConstBuffer(&Transformation, sizeof(Transformation), 0);
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
	Transformation = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * cam->GetView() * cam->GetProjection());
	CB->Update(&Transformation, sizeof(Transformation));
}

void StaticMeshComponent::SetPos(DirectX::XMFLOAT3 Pos)
{
	this->Pos = Pos;
}
