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


MeshMaterial::MeshMaterial(SceneResources* pSceneResources, aiMaterial* mat, std::string RootPath, VertexLayout& VLay)
{
	aiString TexFileName;

	ResourceManager* pRM;

	if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &TexFileName) == aiReturn_SUCCESS)
	{
		pRM = pSceneResources->pTexturePass->GetResourceManager();
		Resources.push_back(pRM->CreateTexture2D(RootPath + TexFileName.C_Str(), 1, true));
		Resources.push_back(pRM->CreateDefaultSampler(2));

		type = MeshMaterial::MaterialType::Texture;
	}
	else
	{
		pRM = pSceneResources->pColorPass->GetResourceManager();

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(color));

		pConstBuffer = pRM->CreateConstBuffer(&color, sizeof(color), 1);

		Resources.push_back(pConstBuffer);

		type = MeshMaterial::MaterialType::Color;
	}
}

MeshMaterial::MaterialType MeshMaterial::GetType()
{
	return type;
}

void MeshMaterial::Bind(Graphics* pGraphics)
{
	for (auto& res : Resources)
		res->Bind(pGraphics);
}

void MeshMaterial::UpdateColor(DirectX::XMFLOAT3 color)
{
	this->color = color;
	pConstBuffer->pData = static_cast<const void*>(&this->color);
	pConstBuffer->DataSize = sizeof(this->color);
}

std::shared_ptr<MeshMaterial> MeshMaterial::GetDefaultMaterial(SceneResources* pSceneResources, VertexLayout& VLay)
{
	std::shared_ptr<MeshMaterial> StaticMaterial = std::make_shared<MeshMaterial>();

	if (VLay.GetSize() >= 3)
	{
		auto pRM = pSceneResources->pTexturePass->GetResourceManager();
		StaticMaterial->Resources.push_back(pRM->CreateTexture2D("C:\\Home\\Visual Projects\\DirectX-12-Engine\\Gifs\\Missing_textures.png", 1, true));
		StaticMaterial->Resources.push_back(pRM->CreateDefaultSampler(2));
		StaticMaterial->type = MeshMaterial::MaterialType::Texture;
	}
	else
		throw new std::runtime_error("Can't create default material");

	return StaticMaterial;
}
