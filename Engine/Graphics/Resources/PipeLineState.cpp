#include "..\..\Headers\Graphics\Resources\PipeLineState.h"
#include "..\..\Headers\Graphics\Resources\Resource.h"
#include "..\..\Headers\Utility.h"
#include "..\..\Headers\Graphics\Error_Check.h"
#include "../../Headers/Graphics.h"


PSO_Layout::PSO_Layout(unsigned int RTC, unsigned int SmpCount) noexcept
	:
	rType(Rasterizer::Default),
	bType(Blender::DefaultValue),
	pType(Topology::Triangle),
	RenderTargetsCount(RTC),
	Depth(false),
	Stencil(false),
	SampleCount(SmpCount)
{

}

void PSO_Layout::DepthState(bool v) noexcept
{
	Depth = v;
}

void PSO_Layout::StencilState(bool v) noexcept
{
	Stencil = v;
}

void PSO_Layout::SetShader(Shader Type, std::string Path) noexcept
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

void PSO_Layout::SetTopology(Topology Type) noexcept
{
	pType = Type;
}

std::string PSO_Layout::GetCode() noexcept
{
	std::string code;
	code += "{" + VS + "," + PS + "," + GS + "," + HS + "," + DS + ":" + std::to_string(static_cast<UINT>(rType)) + "," + std::to_string(static_cast<UINT>(bType)) + "," + std::to_string(static_cast<UINT>(pType))
		+ ":" + std::to_string(Depth) + "," + std::to_string(Stencil) + ":" + std::to_string(RenderTargetsCount) + ":" + std::to_string(SampleCount) + "}";
	return code;
}

PipelineStateObject::PipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay) noexcept
	:
	P_Lay(pLay),
	V_Lay(vLay)

{}

void PipelineStateObject::Initialize(Graphics* pGraphics, RootSignature* pRS)
{
	if (Initialized)
		return;

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	const auto& il = V_Lay.GetDesc();
	psoDesc.InputLayout = { il.data(), static_cast<UINT>(il.size()) };
	psoDesc.pRootSignature = pRS->pRootSignature;
	psoDesc.DepthStencilState = { 0 };
	psoDesc.DepthStencilState.DepthEnable = P_Lay.Depth;
	psoDesc.DSVFormat = pGraphics->GetDSVFormat();
	if (P_Lay.Depth)
	{
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; // Can write depth data to all of the depth/stencil buffer
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // Pixel fragment passes depth test if destination pixel's depth is less than pixel fragment's
	}
	psoDesc.DepthStencilState.StencilEnable = P_Lay.Stencil;
	if (P_Lay.Stencil)
	{
		psoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK; // A default stencil read mask
		psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK; // A default stencil write mask
		const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = // A stencil operation structure
		{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
		psoDesc.DepthStencilState.FrontFace = defaultStencilOp; // Both front and back facing polygons get the same treatment
		psoDesc.DepthStencilState.BackFace = defaultStencilOp;
	}
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = P_Lay.RenderTargetsCount;
	psoDesc.RTVFormats[0] = pGraphics->GetRTVFormat();
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

	free(const_cast<void*>(psoDesc.VS.pShaderBytecode));
	free(const_cast<void*>(psoDesc.PS.pShaderBytecode));
	free(const_cast<void*>(psoDesc.GS.pShaderBytecode));
	free(const_cast<void*>(psoDesc.HS.pShaderBytecode));
	free(const_cast<void*>(psoDesc.DS.pShaderBytecode));

	Initialized = true;
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

std::string PipelineStateObject::GetKey() noexcept
{
	return KeyCode;
}

void PipelineStateObject::SetKey(std::string Key) noexcept
{
	this->KeyCode = Key;
}

void PipelineStateObject::ReadShader(std::string Path, D3D12_SHADER_BYTECODE* pDesc)
{
	BYTE* pData;
	UINT Size;
	ReadDataFromFile(StringToWString(Path).c_str(), &pData, &Size);
	*pDesc = CD3DX12_SHADER_BYTECODE(pData, Size);
}
