#include "..\Headers\Model.h"
#include <sstream>

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(ResourceManager* pRM, std::string ModelPath, float scale)
	:
	Drawable(pRM)
{

	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(ModelPath.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);


	if (pScene == nullptr)
	{
		throw std::exception(imp.GetErrorString());
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		const aiMesh& mesh = *pScene->mMeshes[i];
		Meshes.push_back(new Mesh(pRM, mesh, *pScene->mMaterials[mesh.mMaterialIndex], ModelPath, scale));
	}
}

Engine_API void Model::Update()
{
	
	for (int i = 0; i < Meshes.size(); i++)
	{
		Meshes[i]->Update();
	}
}

Model::~Model()
{
	for (int i = 0; i < Meshes.size(); i++)
	{
		delete Meshes[i];
	}
}
