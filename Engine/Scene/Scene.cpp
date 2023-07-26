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

void Scene::AddModel(std::shared_ptr<StaticMeshComponent> pModel)
{
	/*for (Mesh* m : pModel->Meshes)
	{
		if (pCamera)
			m->Init(pCamera);
		AddDrawable(m);
	}*/

	Models.push_back(pModel);
}

void Scene::AddModels(std::vector<std::shared_ptr<StaticMeshComponent>> Models)
{
	std::copy(this->Models.begin(), this->Models.end(), std::back_inserter(Models));
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
