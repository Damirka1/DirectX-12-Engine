#include "../Headers/Scene/Scene.h"
#include "../Headers/Scene/Model.h"

#include <iterator>

//void Scene::AddDrawable(Drawable* pDrawable)
//{
//	std::shared_ptr<DrawableArray> Array = pDrawable->Array;
//
//	std::string PSOKey = Array->pPipelineStateObject->GetKey();
//	std::string RSKey = Array->pRootSignature->GetKey();
//
//	DrawablesMap[PSOKey].pPipeLineStateObject = Array->pPipelineStateObject;
//	DrawablesMap[PSOKey].RootSignatures[RSKey].pRootSignature = Array->pRootSignature;
//
//	DrawablesMap[PSOKey].RootSignatures[RSKey]
//		.DrawArrayIndexed[Array->key] = Array;
//}

//void Scene::AddModel(std::shared_ptr<StaticMeshComponent> pModel)
//{
//	/*for (Mesh* m : pModel->Meshes)
//	{
//		if (pCamera)
//			m->Init(pCamera);
//		AddDrawable(m);
//	}*/
//
//	Models.push_back(pModel);
//}
//
//void Scene::AddModels(std::vector<std::shared_ptr<StaticMeshComponent>> Models)
//{
//	std::copy(this->Models.begin(), this->Models.end(), std::back_inserter(Models));
//}

Scene::Scene(Graphics* pGraphics)
	:
	SceneResources(pGraphics)
{
}

void Scene::Update()
{
	for (auto model : Models)
		model->Update(pCamera);
}

void Scene::SetCamera(Camera* cam)
{
	if (cam)
		this->pCamera = cam;
	else
		throw std::exception("Camera was nullptr");
}

std::shared_ptr<StaticMeshComponent> Scene::LoadModel(std::string Path, std::string Tag, float Scale)
{
	std::shared_ptr<StaticMeshComponent> _Model = std::make_shared<StaticMeshComponent>(&SceneResources, Path, Scale);
	// TODO: add tag to model

	Models.push_back(_Model);

	return _Model;
}

std::shared_ptr<Plane> Scene::CreatePlane(DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation)
{
	std::shared_ptr<Plane> _Plane = std::make_shared<Plane>(&SceneResources, Pos, Rotation);

	Models.push_back(_Plane);

	return _Plane;
}

std::shared_ptr<Sphere> Scene::CreateSphere(DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation)
{
	std::shared_ptr<Sphere> _Sphere = std::make_shared<Sphere>(&SceneResources, Pos, Rotation);

	Models.push_back(_Sphere);

	return _Sphere;
}

std::shared_ptr<Cube> Scene::CreateCube(DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Rotation)
{
	std::shared_ptr<Cube> _Cube = std::make_shared<Cube>(&SceneResources, Pos, Rotation);

	Models.push_back(_Cube);

	return _Cube;
}

void Scene::InitializeResources()
{
	SceneResources.pColorPass->GetResourceManager()->InitializeResources();
	SceneResources.pTexturePass->GetResourceManager()->InitializeResources();
	SceneResources.pGraphics->Initialize();
	SceneResources.pRTXPass->Initialize(pCamera);
}

//void Scene::InitCamera()
//{
//	if (pCamera)
//		for (auto& PSO : DrawablesMap)
//			for (auto& RS : PSO.second.RootSignatures)
//				for (auto& Drawable : RS.second.DrawArrayIndexed)
//					Drawable.second->InitCamera(pCamera);
//	else
//		throw std::exception("No camera in Scene");
//}
