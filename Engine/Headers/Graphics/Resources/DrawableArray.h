#pragma once
#include <unordered_set>
#include <memory>

class Drawable;
class Graphics;
class VertexBuffer;
class IndexBuffer;
class ResourceManager;
class Camera;
class PipelineStateObject;
class RootSignature;
class HeapDescriptorArray;
class Resource;
class Scene;

class DrawableArray
{
	friend ResourceManager;
	friend Scene;
public:

	DrawableArray(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB, std::string key)
		:
		key(key)
	{
		SetVertexAndIndexBuffers(pVB, pIB);
	}

	void AddDrawable(Drawable* pDrawable) noexcept;
	void RemoveDrawable(Drawable* pDrawable) noexcept;
	void DrawIndexed(Graphics* pGraphics);
	void SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB) noexcept; 
	void SetPSRS(std::shared_ptr<PipelineStateObject> PSO, std::shared_ptr<RootSignature> RS) noexcept
	{
		pPipelineStateObject = PSO;
		pRootSignature = RS;
	}
	void InitCamera(Camera* cam) noexcept;

private:
	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::shared_ptr<PipelineStateObject> pPipelineStateObject;
	std::shared_ptr<RootSignature> pRootSignature;

	std::unordered_set<Drawable*> pDrawables;

	std::shared_ptr<HeapDescriptorArray> pHeap;
	std::vector<std::shared_ptr<Resource>> InitList;

	std::string key;

};