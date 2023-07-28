#pragma once
#include "../Headers/Graphics/Resources/DrawableMeshMaterial.h"
#include "../Headers/Graphics/Resources/VertexLayout.h"
#include "../Headers/Graphics/Resources/RootSignature.h"
#include "../Headers/Graphics/Resources/PipeLineState.h"
#include "../Headers/Graphics/Resources/Resource.h"
#include "../Headers/Graphics/Resources/Buffers/Buffers.h"
#include "../Headers/ResourceManager.h"
#include "../Headers/Scene/SceneResources.h"
#include "../Headers/Input/Camera.h"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>


MeshMaterial::MeshMaterial(SceneResources* pSceneResources, aiMaterial* mat, std::string RootPath)
{
	aiString TexFileName;

	ResourceManager* pRM;

	if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &TexFileName) == aiReturn_SUCCESS)
	{
		pRM = pSceneResources->pTexturePass->GetResourceManager();
		Resources.push_back(pRM->CreateTexture2D(RootPath + TexFileName.C_Str(), 1, true));
		Resources.push_back(pRM->CreateDefaultSampler(2));

		Type = MeshMaterial::MaterialType::Texture;
	}
	else
	{
		pRM = pSceneResources->pColorPass->GetResourceManager();

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(dxColor));

		pConstBuffer = pRM->CreateConstBuffer(&dxColor, sizeof(dxColor), 1);

		Resources.push_back(pConstBuffer);

		Type = MeshMaterial::MaterialType::Color;
	}
}

MeshMaterial::MaterialType MeshMaterial::GetType()
{
	return Type;
}

void MeshMaterial::Bind(Graphics* pGraphics)
{
	for (auto& res : Resources)
		res->Bind(pGraphics);
}

void MeshMaterial::UpdateColor(DirectX::XMFLOAT3 color)
{
	dxColor = color;
	pConstBuffer->pData = static_cast<const void*>(&dxColor);
	pConstBuffer->DataSize = sizeof(dxColor);
}

std::shared_ptr<MeshMaterial> MeshMaterial::GetDefaultMaterial(SceneResources* pSceneResources)
{
	std::shared_ptr<MeshMaterial> StaticMaterial = std::make_shared<MeshMaterial>();

	auto pRM = pSceneResources->pTexturePass->GetResourceManager();
	StaticMaterial->Resources.push_back(pRM->CreateTexture2D("C:\\Home\\Visual Projects\\DirectX-12-Engine\\Gifs\\Missing_textures.png", 1, true));
	StaticMaterial->Resources.push_back(pRM->CreateDefaultSampler(2));
	StaticMaterial->Type = MeshMaterial::MaterialType::Texture;

	return StaticMaterial;
}
