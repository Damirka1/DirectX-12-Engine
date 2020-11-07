#include "Triangle.h"
#include "..\Headers\ResourceManager.h"
#include "..\Headers\Graphics\Resources\BindablesHeader.h"

Triangle::Triangle(Graphics* pGraphics, ResourceManager* pRM)
{
	struct VB
	{
		float x, y;
		float r, g, b;
	};

	VB vb[] = { {-0.25f,  0.25f},
				{ 0.25f,  0.25f},
				{ 0.25f, -0.25f},
				{-0.25f, -0.25f}
	};

	VertexLayout Lay;
	Lay.AddElement("Position", DXGI_FORMAT_R32G32_FLOAT);

	SetVertexbuffer(pRM->CreateVertexBuffer(pGraphics, reinterpret_cast<void*>(vb), sizeof(VB), sizeof(vb), Lay));

	RS_Layout RsLay;
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL).AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	auto RS = pRM->CreateRootSignature(pGraphics, RsLay);

	AddBindable(RS);

	c = { 1.0f, 1.0f, 1.0f };
	pConstBuffer = AddConstBuffer(pGraphics, pRM, &c, sizeof(c), 0);

	PSO_Layout pLay(1, pGraphics->GetFormat());
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("E:\\Visual Projects\\DirectX 12 Engine\\Engine\\Shaders\\VertexShader.cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("E:\\Visual Projects\\DirectX 12 Engine\\Engine\\Shaders\\PixelShader.cso"));

	AddBindable(pRM->CreatePSO(pGraphics, pLay, &Lay, RS.get()));


	std::vector<unsigned int> indecies = { 0,1,2,2,3,0 };

	SetIndexBuffer(pRM->CreateIndexBuffer(pGraphics, indecies));

}

void Triangle::Draw(Graphics* pGraphics)
{
	Drawable::Bind(pGraphics);
	Drawable::DrawIndexed(pGraphics);
}

void Triangle::Update(float r, float g, float b)
{
	c = { r,g,b };
	pConstBuffer->Update(&c, sizeof(c));
}
