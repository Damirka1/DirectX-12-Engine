#pragma once
#ifndef PIPELINESTATEOBJECT_HEADER
#define PIPELINESTATEOBJECT_HEADER
#include "Bindable.h"

class PSO_Layout
{
	friend class PipelineStateObject;
public:
	enum class Shader
	{
		Vertex,
		Pixel,
		Geometry,
		Hull,
		Domain
	};

	enum class Rasterizer
	{
		Default
	};
	enum class Blender
	{
		DefaultValue
	};
	enum class Topology
	{
		Triangle
	};

	PSO_Layout(unsigned int RTC, DXGI_FORMAT Format, unsigned int SmpCount = 1);
	void DepthState(bool v);
	void StencilState(bool v);
	void SetShader(Shader Type, std::string Path);
	void SetTopology(Topology Type);
	std::string GetCode();


private:
	std::string VS{}, PS{}, GS{}, HS{}, DS{};
	Rasterizer rType;
	Blender bType;
	Topology pType;
	bool Depth, Stencil;
	unsigned int RenderTargetsCount;
	DXGI_FORMAT RTV_Format;
	unsigned int SampleCount;
};


class VertexLayout;
class RootSignature;


class PipelineStateObject : public Bindable
{
public:
	PipelineStateObject(Graphics* pGraphics, PSO_Layout& pLay, VertexLayout* vLay, RootSignature* RS);
	void Bind(Graphics* pGraphics) override;
	~PipelineStateObject() override;

private:
	void ReadShader(std::string Path, D3D12_SHADER_BYTECODE* pDesc);

	ID3D12PipelineState* pPipelineStateObject;
};

#endif