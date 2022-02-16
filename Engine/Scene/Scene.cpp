#include "../Headers/Scene/Scene.h"
#include "../Headers/Model.h"

void Scene::AddDrawable(Drawable* pDrawable)
{
	std::shared_ptr<DrawableArray> Array = pDrawable->Array;

	std::string PSOKey = Array->pPipelineStateObject->GetKey();
	std::string RSKey = Array->pRootSignature->GetKey();

	DrawablesMap[PSOKey].pPipeLineStateObject = Array->pPipelineStateObject;
	DrawablesMap[PSOKey].RootSignatures[RSKey].pRootSignature = Array->pRootSignature;

	DrawablesMap[PSOKey].RootSignatures[RSKey]
		.DrawIndexed[Array->key] = Array;
}

void Scene::AddModel(Model* pModel)
{
	for (Mesh* m : pModel->Meshes)
		AddDrawable(m);
}

void Scene::SetCamera(Camera* cam)
{
	if (cam)
		this->pCamera = cam;
	else
		throw std::exception("Camera was nullptr");
}

void Scene::InitCamera()
{
	if (pCamera)
		for (auto& PSO : DrawablesMap)
			for (auto& RS : PSO.second.RootSignatures)
				for (auto& Drawable : RS.second.DrawIndexed)
					Drawable.second->InitCamera(pCamera);
	else
		throw std::exception("No camera in Scene");
}
