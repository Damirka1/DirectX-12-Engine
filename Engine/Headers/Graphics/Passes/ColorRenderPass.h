#pragma once

#include "RenderPass.h"
#include "../../ResourceManager.h"

class ColorRenderPass : public RenderPass
{
public:
	ColorRenderPass(Graphics* pGraphics)
		:
		RenderPass(pGraphics)
	{
		RS_Layout RLay;

		RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
			.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

		RLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
			.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

		VertexLayout Lay;
		Lay.AddElement("Position", DXGI_FORMAT_R32G32B32_FLOAT);
		Lay.AddElement("NormCoord", DXGI_FORMAT_R32G32B32_FLOAT);
		Lay.AddElement("TanCoord", DXGI_FORMAT_R32G32B32_FLOAT);
		Lay.AddElement("BiTanCoord", DXGI_FORMAT_R32G32B32_FLOAT);

		// Set 4 render targets
		PSO_Layout pLay(4);
		pLay.DepthState(true);

		pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\PixelShader.cso"));
		pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("Shaders\\VertexShader.cso"));

		pRootSignature = pRM->CreateRootSignature(RLay);
		pPipelineStateObject = pRM->CreatePipelineStateObject(pLay, Lay, pRootSignature);
	}

};