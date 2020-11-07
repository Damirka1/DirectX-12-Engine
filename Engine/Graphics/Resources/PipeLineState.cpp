#include "..\..\Headers\Graphics\Resources\PipeLineState.h"
#include "..\..\Headers\Graphics\Resources\VertexLayout.h"
#include "..\..\Headers\Graphics\Resources\RootSignature.h"
#include "..\..\Headers\Utility.h"
#include "..\..\Headers\Graphics\Error_Check.h"


PSO_Layout::PSO_Layout(unsigned int RTC, DXGI_FORMAT Format, unsigned int SmpCount)
	:
	rType(Rasterizer::Default),
	bType(Blender::DefaultValue),
	pType(Topology::Triangle),
	RenderTargetsCount(RTC),
	RTV_Format(Format),
	Depth(false),
	Stencil(false),
	SampleCount(SmpCount)
{

}

void PSO_Layout::DepthState(bool v)
{
	Depth = v;
}

void PSO_Layout::StencilState(bool v)
{
	Stencil = v;
}

void PSO_Layout::SetShader(Shader Type, std::string Path)
{
	switch (Type)
	{
	case PSO_Layout::Shader::Vertex:
		VS = Path;
		break;
	case PSO_Layout::Shader::Pixel:
		PS = Path;
		break;
	case PSO_Layout::Shader::Geometry:
		GS = Path;
		break;
	case PSO_Layout::Shader::Hull:
		HS = Path;
		break;
	case PSO_Layout::Shader::Domain:
		DS = Path;
		break;
	}
}

void PSO_Layout::SetTopology(Topology Type)
{
	pType = Type;
}

std::string PSO_Layout::GetCode()
{
	std::string code;
	code += "{" + VS + "," + PS + "," + GS + "," + HS + "," + DS + ":" + std::to_string(static_cast<UINT>(rType)) + "," + std::to_string(static_cast<UINT>(bType)) + "," + std::to_string(static_cast<UINT>(pType))
		+ ":" + std::to_string(Depth) + "," + std::to_string(Stencil) + ":" + std::to_string(RenderTargetsCount) + ":" + std::to_string(RTV_Format) + ":" + std::to_string(SampleCount) + "}";
	return code;
}

PipelineStateObject::PipelineStateObject(Graphics* pGraphics, PSO_Layout& pLay, VertexLayout* vLay, RootSignature* RS)
{
	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	const auto& il = vLay->GetDesc();
	psoDesc.InputLayout = { il.data(), static_cast<UINT>(il.size()) };
	psoDesc.pRootSignature = RS->pRootSignature;

	psoDesc.DepthStencilState.DepthEnable = pLay.Depth;
	psoDesc.DepthStencilState.StencilEnable = pLay.Stencil;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = pLay.RenderTargetsCount;
	psoDesc.RTVFormats[0] = pLay.RTV_Format;
	psoDesc.SampleDesc.Count = pLay.SampleCount;


	switch (pLay.rType)
	{
	case PSO_Layout::Rasterizer::Default:
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		break;
	}

	switch (pLay.bType)
	{
	case PSO_Layout::Blender::DefaultValue:
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		break;
	}

	switch (pLay.pType)
	{
	case PSO_Layout::Topology::Triangle:
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	}


	if (pLay.VS.length() > 0)
	{
		ReadShader(pLay.VS, &psoDesc.VS);
	}
	if (pLay.PS.length() > 0)
	{
		ReadShader(pLay.PS, &psoDesc.PS);
	}
	if (pLay.GS.length() > 0)
	{
		ReadShader(pLay.GS, &psoDesc.GS);
	}
	if (pLay.HS.length() > 0)
	{
		ReadShader(pLay.HS, &psoDesc.HS);
	}
	if (pLay.DS.length() > 0)
	{
		ReadShader(pLay.DS, &psoDesc.DS);
	}

	Error_Check(
		pGraphics->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineStateObject))
	);
}

void PipelineStateObject::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->SetPipelineState(pPipelineStateObject);
}

PipelineStateObject::~PipelineStateObject()
{
	pPipelineStateObject->Release();
	pPipelineStateObject = nullptr;
}

void PipelineStateObject::ReadShader(std::string Path, D3D12_SHADER_BYTECODE* pDesc)
{
	BYTE* pData;
	UINT Size;
	ReadDataFromFile(StringToWString(Path).c_str(), &pData, &Size);
	*pDesc = CD3DX12_SHADER_BYTECODE(pData, Size);
}
