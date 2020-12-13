#include "Rectangle.h"

Rect::Rect(ResourceManager* pRM, std::string Name, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size)
{

	Init(Name, Pos, Size);

	struct VB
	{
		float x, y;
	};

	VB vb[] = {
		{-1.0f * Size.x, -1.0f * Size.y},
		{-1.0f * Size.x,  1.0f * Size.y},
		{ 1.0f * Size.x,  1.0f * Size.y},
		{ 1.0f * Size.x, -1.0f * Size.y}
	};

	VertexLayout Lay;
	Lay.AddElement("Position", DXGI_FORMAT_R32G32_FLOAT);


	std::vector<unsigned int> indecies = {
				0,1,2, 2,3,0
	};

	SetVertexAndIndexBuffers(pRM->CreateVertexBuffer(this, reinterpret_cast<void*>(vb), sizeof(VB), sizeof(vb), Lay),
		pRM->CreateIndexBuffer(this, indecies));


	PSO_Layout pLay(1);
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("Shaders\\UI_VertexShader.cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\UI_PixelShader.cso"));
	std::string PSO_key = pRM->CreatePSO(pLay, &Lay, true);


	RS_Layout RsLay;
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	pRM->CreateRootSignature(PSO_key, RsLay, this);

	pBufferColor = pRM->CreateConstBuffer(this, &Color, sizeof(Color), 1, 0, 0);

	Cbuff.Pos = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat2(&Pos)));
	Cbuff.Projection = pRM->GetProjectionForUI();

	pBufferProjection = pRM->CreateConstBuffer(this, &Cbuff, sizeof(Cbuff), 0, 0, 0);
}

Rect::~Rect()
{
}

