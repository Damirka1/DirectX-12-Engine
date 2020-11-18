#include "Triangle.h"
#include "..\Headers\ResourceManager.h"
#include "..\Headers\Graphics\Resources\BindablesHeader.h"

Triangle::Triangle(Graphics* pGraphics, ResourceManager* pRM)
{
	struct VB
	{
		float x, y;
		float U, V;
	};

	VB vb[] = { {-0.5f,  0.4f, 0.0f, 0.0f},
				{ 0.5f,  0.4f, 1.0f, 0.0f},
				{ 0.5f, -0.4f, 1.0f, 1.0f},
				{-0.5f, -0.4f, 0.0f, 1.0f}
	};

	VertexLayout Lay;
	Lay.AddElement("Position", DXGI_FORMAT_R32G32_FLOAT);
	Lay.AddElement("TexCoord", DXGI_FORMAT_R32G32_FLOAT);

	SetVertexbuffer(pRM->CreateVertexBuffer(pGraphics, reinterpret_cast<void*>(vb), sizeof(VB), sizeof(vb), Lay));

	RS_Layout RsLay;
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL).AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1);

	auto RS = pRM->CreateRootSignature(pGraphics, RsLay);

	auto pHeap = RS->GetHeapArray();

	b.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	b.pos = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity());
	pConstBuffer = pRM->CreateConstBuffer(pGraphics, &b, sizeof(b), pHeap->GetCPUHandle(0,0,0));

	pTexture = pRM->CreateTexture2D(pGraphics, "Image\\bugatti-la-voiture-noire-roadster-rendering-lead-image.jpg", pHeap->GetCPUHandle(0,1,0));
	pSampler = pRM->CreateDefaultSampler(pGraphics, pHeap->GetCPUHandle(1, 0, 0));

	AddBindable(RS);
	AddBindable(std::move(pHeap));

	PSO_Layout pLay(1, pGraphics->GetFormat());
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("..\\Engine\\Shaders\\VertexShader.cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("..\\Engine\\Shaders\\PixelShader.cso"));

	AddBindable(pRM->CreatePSO(pGraphics, pLay, &Lay, RS.get()));


	std::vector<unsigned int> indecies = { 0,1,2,2,3,0 };

	SetIndexBuffer(pRM->CreateIndexBuffer(pGraphics, indecies));

	

}

void Triangle::Draw(Graphics* pGraphics)
{
	Drawable::Bind(pGraphics);
	Drawable::DrawIndexed(pGraphics);
}

void Triangle::Update(float r, float g, float b, float up, float left)
{
	this->b.pos *= DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(left, up, 0.0f));
	//this->b.color = { r,g,b, 1.0f };

	pConstBuffer->Update(&this->b.pos, sizeof(this->b.pos));
}

Triangle::~Triangle()
{
}
