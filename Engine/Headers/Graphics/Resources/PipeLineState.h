#pragma once
#ifndef PIPELINESTATEOBJECT_HEADER
#define PIPELINESTATEOBJECT_HEADER

#include "Resource.h"

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

	PSO_Layout(unsigned int RTC, unsigned int SmpCount = 1) noexcept;
	void DepthState(bool v) noexcept;
	void StencilState(bool v) noexcept;
	void SetShader(Shader Type, std::string Path) noexcept;
	void SetTopology(Topology Type) noexcept;
	std::string GetCode() noexcept;


private:
	mutable std::string VS{}, PS{}, GS{}, HS{}, DS{};
	mutable Rasterizer rType;
	mutable Blender bType;
	mutable Topology pType;
	mutable bool Depth, Stencil;
	unsigned int RenderTargetsCount;
	unsigned int SampleCount;
};


#include "VertexLayout.h"
#include "RootSignature.h"

class PipelineStateObject : public Resource
{
	friend class ResourceManager;
public:
	PipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay) noexcept;
	void Initialize(Graphics* pGraphics, RootSignature* pRS);
	void Bind(Graphics* pGraphics) override;
	~PipelineStateObject();
	std::string GetKey() noexcept;

private:
	void SetKey(std::string Key) noexcept;
	void ReadShader(std::string Path, D3D12_SHADER_BYTECODE* pDesc);

	ID3D12PipelineState* pPipelineStateObject;
	PSO_Layout P_Lay;
	VertexLayout V_Lay;
	std::string KeyCode;
};

#endif