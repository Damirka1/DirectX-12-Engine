#pragma once
#include "../Headers/Graphics/Resources/DrawableMeshMaterial.h"
#include "../Headers/Graphics/Resources/VertexLayout.h"
#include "../Headers/Graphics/Resources/RootSignature.h"
#include "../Headers/Graphics/Resources/PipeLineState.h"
#include "../Headers/Graphics/Resources/Resource.h"
#include "../Headers/Graphics/Resources/Buffers/Buffers.h"
#include "../Headers/ResourceManager.h"
#include "../Headers/Input/Camera.h"


MeshMaterial::MeshMaterial(ResourceManager* pRM, aiMaterial* mat)
{
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

	StaticMaterial->pPipelineStateObject = pRM->CreatePipelineStateObject(pLay, VLay);
	StaticMaterial->pRootSignature = pRM->CreateRootSignature(RLay);

	return StaticMaterial;
}
