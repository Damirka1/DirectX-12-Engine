#include "..\..\Headers\Graphics\Resources\PipeLineState.h"
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

PipelineStateObject::PipelineStateObject(PSO_Layout& pLay, VertexLayout* vLay)
	:
	P_Lay(pLay),
	V_Lay(*vLay)

{}

void PipelineStateObject::Initialize(Graphics* pGraphics, RootSignature* pRS)	
{
	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	const auto& il = V_Lay.GetDesc();
	psoDesc.InputLayout = { il.data(), static_cast<UINT>(il.size()) };
	psoDesc.pRootSignature = pRS->pRootSignature;

	psoDesc.DepthStencilState.DepthEnable = P_Lay.Depth;
	psoDesc.DepthStencilState.StencilEnable = P_Lay.Stencil;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = P_Lay.RenderTargetsCount;
	psoDesc.RTVFormats[0] = P_Lay.RTV_Format;
	psoDesc.SampleDesc.Count = P_Lay.SampleCount;


	switch (P_Lay.rType)
	{
	case PSO_Layout::Rasterizer::Default:
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		break;
	}

	switch (P_Lay.bType)
	{
	case PSO_Layout::Blender::DefaultValue:
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		break;
	}

	switch (P_Lay.pType)
	{
	case PSO_Layout::Topology::Triangle:
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	}


	if (P_Lay.VS.length() > 0)
	{
		ReadShader(P_Lay.VS, &psoDesc.VS);
	}
	if (P_Lay.PS.length() > 0)
	{
		ReadShader(P_Lay.PS, &psoDesc.PS);
	}
	if (P_Lay.GS.length() > 0)
	{
		ReadShader(P_Lay.GS, &psoDesc.GS);
	}
	if (P_Lay.HS.length() > 0)
	{
		ReadShader(P_Lay.HS, &psoDesc.HS);
	}
	if (P_Lay.DS.length() > 0)
	{
		ReadShader(P_Lay.DS, &psoDesc.DS);
	}

	Error_Check(
		pGraphics->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineStateObject))
	);

	this->Initialized = true;
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
