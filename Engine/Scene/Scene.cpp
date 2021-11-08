#include "../Headers/Scene/Scene.h"
#include "../Headers/Model.h"

void Scene::AddDrawable(Drawable* pDrawable)
{
	std::string PSOKey = pDrawable->pPipelineStateObject->GetKey();
	std::string RSKey = pDrawable->pRootSignature->GetKey();

	std::shared_ptr<PipelineStateObject> pPipelineStateObject = pDrawable->pPipelineStateObject;
	std::shared_ptr<RootSignature> pRootSignature = pDrawable->pRootSignature;

	DrawablesMap[PSOKey].pPipeLineStateObject = pPipelineStateObject;
	DrawablesMap[PSOKey].RootSignatures[RSKey].pRootSignature = pRootSignature;

	DrawablesMap[PSOKey].RootSignatures[RSKey]
		.pDrawablesToInitialize.push_back(pDrawable);
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
					Drawable.second.InitCamera(pCamera);
	else
		throw std::exception("No camera in Scene");
}
