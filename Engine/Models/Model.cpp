//#include "../Headers/Scene/Model.h"
//#include <sstream>
//
//#include "assimp/Importer.hpp"
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//Model::Model(ResourceManager* pRM, std::string ModelPath, float scale)
//{
//	Assimp::Importer imp;
//	const auto pScene = imp.ReadFile(ModelPath.c_str(),
//		aiProcess_Triangulate |
//		//aiProcess_JoinIdenticalVertices |
//		aiProcess_ConvertToLeftHanded |
//		aiProcess_GenNormals |
//		aiProcess_CalcTangentSpace
//	);
//
//	if (pScene == nullptr)
//		throw std::exception(imp.GetErrorString());
//
//	for (size_t i = 0; i < pScene->mNumMeshes; i++)
//	{
//		aiMesh* mesh = pScene->mMeshes[i];
//		Meshes.push_back(new Mesh(pRM, mesh, pScene->mMaterials[mesh->mMaterialIndex], ModelPath, scale));
//	}
//}
//
//Engine_API void Model::Update()
//{
//	if (!Meshes[0]->IsReady())
//		return;
//
//	for (Mesh* m : Meshes)
//		m->Update();
//}
//
//Engine_API void Model::SetPos(DirectX::XMFLOAT3 Pos)
//{
//	Position = Pos;
//	for (Mesh* m : Meshes)
//		m->SetPos(Position);
//}
//
//Engine_API DirectX::XMFLOAT3 Model::GetPos()
//{
//	return Position;
//}
//
//Model::~Model()
//{
//	for (int i = 0; i < Meshes.size(); i++)
//		delete Meshes[i];
//}
