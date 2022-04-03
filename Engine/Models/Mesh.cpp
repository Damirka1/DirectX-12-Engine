//#include "../Headers/Scene/Mesh.h"
//
//#include "assimp/Importer.hpp"
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//Mesh::Mesh(ResourceManager* pRM, aiMesh* m, aiMaterial* material, std::filesystem::path path, float scale)
//{
//	RS_Layout RsLay;
//	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
//		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);
//
//	VertexLayout Lay;
//	Lay.AddElement("Position", DXGI_FORMAT_R32G32B32_FLOAT);
//	Lay.AddElement("NormCoord", DXGI_FORMAT_R32G32B32_FLOAT);
//
//	std::string ShaderCode;
//	std::string RootPath = path.parent_path().string() + "\\";
//
//	bool diffuse = false;
//
//	// diffuse
//	{
//		aiString TexFileName;
//		bool hasAlpha = false;
//		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &TexFileName) == aiReturn_SUCCESS)
//		{
//			ShaderCode += "Dif";
//			diffuse = true;
//			Lay.AddElement("TexCoord", DXGI_FORMAT_R32G32_FLOAT);
//			RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
//				.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
//			RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
//				.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1);
//
//			//AddResource(pRM->CreateTexture2D(this, RootPath + TexFileName.C_Str(), 1, 0, 0));
//			//AddResource(pRM->CreateDefaultSampler(this, 2, 0, 0));
//		}
//		else
//		{
//			material->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(color));
//			RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
//				.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);
//			//AddResource(pRM->CreateConstBuffer(this, &color, sizeof(color), 1, 0, 0));
//		}
//	}
//
//	std::vector<FLOAT> Buffer;
//	std::vector<UINT> Indecies;
//
//	for (size_t i = 0, j = 0, k = 0; i < m->mNumVertices; i++, j++, k++)
//	{
//		aiVector3D v = m->mVertices[i];
//
//		Buffer.push_back(v.x * scale);
//		Buffer.push_back(v.y * scale);
//		Buffer.push_back(v.z * scale);
//
//		aiVector3D vn = m->mNormals[k];
//		Buffer.push_back(vn.x);
//		Buffer.push_back(vn.y);
//		Buffer.push_back(vn.z);
//
//		if (diffuse)
//		{
//			if (m->HasTextureCoords(0))
//			{
//				aiVector3D vt = m->mTextureCoords[0][j];
//				Buffer.push_back(vt.x);
//				Buffer.push_back(vt.y);
//			}
//		}
//	}
//
//	for (size_t i = 0; i < m->mNumFaces; i++)
//	{
//		aiFace face = m->mFaces[i];
//		if (face.mNumIndices == 3)
//		{
//			Indecies.push_back(face.mIndices[0]);
//			Indecies.push_back(face.mIndices[1]);
//			Indecies.push_back(face.mIndices[2]);
//		}
//		else
//			throw std::exception("Not triangulated mesh");
//	}
//	
//	int count = diffuse ? 3 + 3 + 2 : 3 + 3;
//
//	PSO_Layout pLay(1);
//	pLay.DepthState(true);
//	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("Shaders\\VertexShader" + ShaderCode + ".cso"));
//	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\PixelShader" + ShaderCode + ".cso"));
//
//	//pRM->CreateAllMeshResources(this, &Indecies, Buffer.data(), sizeof(float) * count, sizeof(float) * Buffer.size(), Lay, m->mNumVertices, 0, pLay, RsLay, Lay);
//
//	//Transformation = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * *pCamera.View * *pCamera.Projection);
//	//pConstBuffer = pRM->CreateConstBuffer(this, &Transformation, sizeof(Transformation), 0, 0, 0);
//
//	//// diffuse
//	//{
//	//	aiString TexFileName;
//	//	bool hasAlpha = false;
//	//	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &TexFileName) == aiReturn_SUCCESS)
//	//	{
//	//		AddResource(pRM->CreateTexture2D(this, RootPath + TexFileName.C_Str(), 1, 0, 0, true));
//	//		AddResource(pRM->CreateDefaultSampler(this, 2, 0, 0));
//	//	}
//	//	else
//	//	{
//	//		material->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(color));
//	//		AddResource(pRM->CreateConstBuffer(this, &color, sizeof(color), 1, 0, 0));
//	//	}
//	//}
//}
//
//void Mesh::Update()
//{
//	Transformation = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * *pCamera.View * *pCamera.Projection);
//	pConstBuffer->Update(&Transformation, sizeof(Transformation));
//}
