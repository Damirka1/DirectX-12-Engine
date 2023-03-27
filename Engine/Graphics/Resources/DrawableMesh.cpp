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
	bool HasTexCoords = m->HasTextureCoords(0);

	if(HasTexCoords)
		Lay.AddElement("TexCoord", DXGI_FORMAT_R32G32_FLOAT);

	std::vector<FLOAT> Buffer;
	std::vector<UINT> Indecies;

	for (size_t i = 0, j = 0, k = 0; i < m->mNumVertices; i++, j++, k++)
	{
		aiVector3D v = m->mVertices[i];

		Buffer.push_back(v.x * scale);
		Buffer.push_back(v.y * scale);
		Buffer.push_back(v.z * scale);

		aiVector3D vn = m->mNormals[k];
		Buffer.push_back(vn.x);
		Buffer.push_back(vn.y);
		Buffer.push_back(vn.z);

		if (HasTexCoords)
		{
			aiVector3D vt = m->mTextureCoords[0][j];
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

	int count = HasTexCoords ? 3 + 3 + 2 : 3 + 3;

	pVertexBuffer = pRM->CreateVertexBuffer(Buffer.data(), sizeof(float) * count, sizeof(float) * Buffer.size(), Lay, m->mNumVertices);
	pIndexBuffer = pRM->CreateIndexBuffer(&Indecies);

	if (!mat)
		Material = MeshMaterial::GetDefaultMaterial(pRM, Lay);
	else
		Material = std::make_shared<MeshMaterial>(pRM, mat, path.parent_path().string() + "\\", Lay);
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
