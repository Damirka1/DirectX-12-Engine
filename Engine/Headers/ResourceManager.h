#pragma once
#ifndef RESOURCE_MANAGER_HEADER
#define RESOURCE_MANAGER_HEADER
#include <unordered_map>
#include <string>
#include <memory>

class Bindable;
class VertexBuffer;
class Graphics;
class VertexLayout;
class IndexBuffer;
class RootSignature;
class PipelineStateObject;
class PSO_Layout;
class RS_Layout;

class ResourceManager
{
public:
	std::shared_ptr<VertexBuffer> CreateVertexBuffer(Graphics* pGraphics, void* pData, const unsigned int Stride, unsigned int DataSize, VertexLayout Lay, unsigned int Slot = 0);
	std::shared_ptr<IndexBuffer> CreateIndexBuffer(Graphics* pGraphics, std::vector<unsigned int> Indecies);
	std::shared_ptr<RootSignature> CreateNullRootSignature(Graphics* pGraphics);
	std::shared_ptr<RootSignature> CreateRootSignature(Graphics* pGraphics, RS_Layout& Lay);
	std::shared_ptr<PipelineStateObject> CreatePSO(Graphics* pGraphics, PSO_Layout& pLay, VertexLayout* vLay, RootSignature* RS);

private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> Bindables;
};

#endif
