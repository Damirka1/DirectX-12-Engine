#include "../Headers/Mesh.h"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Mesh::Mesh(ResourceManager* pRM, const aiMesh& m, const aiMaterial& material, std::filesystem::path path, float scale)
	:
	Drawable(pRM)
{
	RS_Layout RsLay;
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	VertexLayout Lay;
	Lay.AddElement("Position", DXGI_FORMAT_R32G32B32_FLOAT);
	Lay.AddElement("NormCoord", DXGI_FORMAT_R32G32B32_FLOAT);


	std::string ShaderCode;
	std::string RootPath = path.parent_path().string() + "\\";

	// diffuse
	{
		aiString TexFileName;
		bool hasAlpha = false;
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &TexFileName) == aiReturn_SUCCESS)
		{
			ShaderCode += "Dif";
			Lay.AddElement("TexCoord", DXGI_FORMAT_R32G32_FLOAT);
			RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
				.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
			RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
				.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1);

			pRM->CreateTexture2D(this, RootPath + TexFileName.C_Str(), 1, 0, 0);
			pRM->CreateDefaultSampler(this, 2, 0, 0);
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(color));
			RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
				.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);
			pRM->CreateConstBuffer(this, &color, sizeof(color), 1, 0, 0);
		}
	}

	std::vector<float> Buffer;
	std::vector<unsigned int> Indecies;

	int i = 0, j = 0, k = 0;
	while (true)
	{
		Buffer.push_back(m.mVertices[i].x * scale);
		Buffer.push_back(m.mVertices[i].y * scale);
		Buffer.push_back(m.mVertices[i++].z * scale);

		Buffer.push_back(m.mNormals[k].x);
		Buffer.push_back(m.mNormals[k].y);
		Buffer.push_back(m.mNormals[k++].z);

		Buffer.push_back(m.mTextureCoords[0][j].x);
		Buffer.push_back(m.mTextureCoords[0][j++].y);

		if (i == m.mNumVertices)
			break;
	}


	for (int end = m.mNumFaces, i = 0; i < end; i++)
	{
		for (int end2 = m.mFaces[i].mNumIndices, j = 0; j < end2; j++)
		{
			Indecies.push_back(m.mFaces[i].mIndices[j]);
		}
	}

	SetVertexAndIndexBuffers(pRM->CreateVertexBuffer(this, Buffer.data(), sizeof(float) * (3 + 2 + 3), sizeof(float) * (UINT)Buffer.size(), Lay),
		pRM->CreateIndexBuffer(this, Indecies));

	PSO_Layout pLay(1);
	pLay.DepthState(true);
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("Shaders\\VertexShader" + ShaderCode + ".cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\PixelShader" + ShaderCode + ".cso"));
	std::string PSO_key = pRM->CreatePSO(pLay, Lay);

	std::string RS_key = pRM->CreateRootSignature(this, PSO_key, RsLay);

	Drawable::Pos = { 0.0f, 0.0f, 10.0f };
	Transformation = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * *pCamera.View * *pCamera.Projection);
	pConstBuffer = pRM->CreateConstBuffer(this, &Transformation, sizeof(Transformation), 0, 0, 0);

}

void Mesh::Update()
{
	Transformation = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * *pCamera.View * *pCamera.Projection);
	pConstBuffer->Update(&Transformation, sizeof(Transformation));
}
