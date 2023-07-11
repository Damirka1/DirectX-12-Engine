#pragma once
#include "../Headers/Graphics/Resources/DrawableMeshMaterial.h"
#include "../Headers/Graphics/Resources/VertexLayout.h"
#include "../Headers/Graphics/Resources/RootSignature.h"
#include "../Headers/Graphics/Resources/PipeLineState.h"
#include "../Headers/Graphics/Resources/Resource.h"
#include "../Headers/Graphics/Resources/Buffers/Buffers.h"
#include "../Headers/ResourceManager.h"
#include "../Headers/Input/Camera.h"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>


MeshMaterial::MeshMaterial(ResourceManager* pRM, aiMaterial* mat, std::string RootPath, VertexLayout& VLay)
{
	std::string ShaderCode = "";
	RS_Layout RLay;

	RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	aiString TexFileName;

	// Set 4 render targets
	PSO_Layout pLay(4);
	pLay.DepthState(true);

	if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &TexFileName) == aiReturn_SUCCESS)
	{
		ShaderCode += "Dif";
		
		RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
			.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
		RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
			.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1);

		Resources.push_back(pRM->CreateTexture2D(RootPath + TexFileName.C_Str(), 1, true));
		Resources.push_back(pRM->CreateDefaultSampler(2));

		pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\PixelatePixelShader" + ShaderCode + ".cso"));
	}
	else
	{
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(color));
		RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
			.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

		Resources.push_back(pRM->CreateConstBuffer(&color, sizeof(color), 1));

		pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\PixelShader" + ShaderCode + ".cso"));
	}

	
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("Shaders\\VertexShader" + ShaderCode + ".cso"));
	

	pRootSignature = pRM->CreateRootSignature(RLay);
	pPipelineStateObject = pRM->CreatePipelineStateObject(pLay, VLay, pRootSignature);

}

void MeshMaterial::Bind(Graphics* pGraphics)
{
	// 
	pRootSignature->Bind(pGraphics);
	pPipelineStateObject->Bind(pGraphics);
	//

	for (auto& res : Resources)
		res->Bind(pGraphics);
}

std::shared_ptr<MeshMaterial> MeshMaterial::GetDefaultMaterial(ResourceManager* pRM, VertexLayout& VLay)
{
	std::shared_ptr<MeshMaterial> StaticMaterial = std::make_shared<MeshMaterial>();

	std::string ShaderCode = "";
	RS_Layout RLay;

	RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	if (VLay.GetSize() >= 3)
	{
		ShaderCode += "Dif";

		RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
			.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);

		RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
			.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1);

		StaticMaterial->Resources.push_back(pRM->CreateTexture2D("C:\\Home\\Visual Projects\\DirectX-12-Engine\\Gifs\\Missing_textures.png", 1, true));
		StaticMaterial->Resources.push_back(pRM->CreateDefaultSampler(2));
	}

	PSO_Layout pLay(1);
	pLay.DepthState(true);
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("Shaders\\VertexShader" + ShaderCode + ".cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\PixelShader" + ShaderCode + ".cso"));

	StaticMaterial->pRootSignature = pRM->CreateRootSignature(RLay);
	StaticMaterial->pPipelineStateObject = pRM->CreatePipelineStateObject(pLay, VLay, StaticMaterial->pRootSignature);

	return StaticMaterial;
}
