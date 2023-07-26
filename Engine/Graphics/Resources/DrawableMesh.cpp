#include "../../Headers/Graphics/Resources/DrawableMesh.h"
#include "../../Headers/Graphics/Resources/Buffers/Buffers.h"
#include "../../Headers/Graphics/Resources/VertexLayout.h"
#include "../../Headers/Graphics/Resources/DrawableMeshMaterial.h"
#include "../../Headers/ResourceManager.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

DrawableMesh::DrawableMesh(ResourceManager* pRM, aiMesh* m, aiMaterial* mat, std::filesystem::path path, float scale)
{
	VertexLayout Lay;
	Lay.AddElement("Position", DXGI_FORMAT_R32G32B32_FLOAT);
	Lay.AddElement("NormCoord", DXGI_FORMAT_R32G32B32_FLOAT);
	Lay.AddElement("TanCoord", DXGI_FORMAT_R32G32B32_FLOAT);
	Lay.AddElement("BiTanCoord", DXGI_FORMAT_R32G32B32_FLOAT);
	bool HasTexCoords = m->HasTextureCoords(0);

	std::vector<FLOAT> Buffer;
	std::vector<UINT> Indecies;

	// Reserve space for vertecies, normals, tangents and bitangents
	int reserveSpace = (m->mNumVertices * 3) * 4;

	if (HasTexCoords)
	{
		Lay.AddElement("TexCoord", DXGI_FORMAT_R32G32_FLOAT);

		Buffer.reserve(reserveSpace + m->mNumVertices * 2);
	}
	else 
		Buffer.reserve(reserveSpace);

	for (size_t i = 0; i < m->mNumVertices; i++)
	{
		aiVector3D v = m->mVertices[i];

		Buffer.push_back(v.x * scale);
		Buffer.push_back(v.y * scale);
		Buffer.push_back(v.z * scale);

		aiVector3D vn = m->mNormals[i];
		Buffer.push_back(vn.x);
		Buffer.push_back(vn.y);
		Buffer.push_back(vn.z);

		aiVector3D vtn = m->mTangents[i];
		Buffer.push_back(vtn.x);
		Buffer.push_back(vtn.y);
		Buffer.push_back(vtn.z);

		aiVector3D vbtn = m->mBitangents[i];
		Buffer.push_back(vbtn.x);
		Buffer.push_back(vbtn.y);
		Buffer.push_back(vbtn.z);

		if (HasTexCoords)
		{
			aiVector3D vt = m->mTextureCoords[0][i];
			Buffer.push_back(vt.x);
			Buffer.push_back(vt.y);
		}
	}

	if(m->mFaces[0].mNumIndices != 3)
		throw std::exception("Not triangulated mesh");

	for (size_t i = 0; i < m->mNumFaces; i++)
	{
		aiFace face = m->mFaces[i];
		Indecies.push_back(face.mIndices[0]);
		Indecies.push_back(face.mIndices[1]);
		Indecies.push_back(face.mIndices[2]);	
	}

	int count = HasTexCoords ? 3 * 4 + 2 : 3 * 4;

	pVertexBuffer = pRM->CreateVertexBuffer(Buffer.data(), sizeof(float) * count, sizeof(float) * Buffer.size(), Lay, m->mNumVertices);
	pIndexBuffer = pRM->CreateIndexBuffer(&Indecies);

	//Material = MeshMaterial::GetDefaultMaterial(pRM, Lay);
	if (!mat)
		Material = MeshMaterial::GetDefaultMaterial(pRM, Lay);
	else
		Material = std::make_shared<MeshMaterial>(pRM, mat, path.parent_path().string() + "\\", Lay);

	this->Lay = Lay;
}

void DrawableMesh::BindMaterial(Graphics* pGraphics)
{
	Material->Bind(pGraphics);
}

void DrawableMesh::Draw(Graphics* pGraphics)
{
	pVertexBuffer->Bind(pGraphics);
	pIndexBuffer->Bind(pGraphics);

	pGraphics->GetCommandList()->DrawIndexedInstanced(pIndexBuffer->GetIndeciesCount(), 1, 0, 0, 0);
}

void DrawableMesh::UpdateColor(DirectX::XMFLOAT3 color)
{
	Material->UpdateColor(color);
}
