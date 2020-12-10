#include "Triangle.h"


Triangle::Triangle(std::string TexturePath, ResourceManager* pRM)
{
	struct VB
	{
		float x, y, z;
		float U, V;
	};

	VB vb[] = { {-0.5f,  0.4f, 0.0f, 0.0f, 0.0f},
				{ 0.5f,  0.4f, 0.0f, 1.0f, 0.0f},
				{ 0.5f, -0.4f, 0.0f, 1.0f, 1.0f},
				{-0.5f, -0.4f, 0.0f, 0.0f, 1.0f}
	};

	VertexLayout Lay;
	Lay.AddElement("Position", DXGI_FORMAT_R32G32B32_FLOAT);
	Lay.AddElement("TexCoord", DXGI_FORMAT_R32G32_FLOAT);

	std::vector<unsigned int> indecies = { 0,1,2,2,3,0 };

	SetVertexAndIndexBuffers(pRM->CreateVertexBuffer(this, reinterpret_cast<void*>(vb), sizeof(VB), sizeof(vb), Lay),
							 pRM->CreateIndexBuffer(this, indecies));

	PSO_Layout pLay(1);
	pLay.DepthState(true);
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("..\\Engine\\Shaders\\VertexShader.cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("..\\Engine\\Shaders\\PixelShader.cso"));
	std::string PSO_key = pRM->CreatePSO(pLay, &Lay);


	RS_Layout RsLay;
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL).
		AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1);

	std::string RS_key =  pRM->CreateRootSignature(PSO_key, RsLay, this);

	b.pos = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity());
	pConstBuffer = pRM->CreateConstBuffer(this, &b.pos, sizeof(b.pos), 0,0,0);
	pConstBuffer->SetKeys(PSO_key, RS_key);

	pTexture = pRM->CreateTexture2D(this, TexturePath, 0,1,0);
	pTexture->SetKeys(PSO_key, RS_key);
	pSampler = pRM->CreateDefaultSampler(this, 1, 0, 0);
	pSampler->SetKeys(PSO_key, RS_key);

}

void Triangle::Update(float up, float left, float forward)
{
	this->b.pos *= DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(left, up, forward));

	pConstBuffer->Update(&this->b.pos, sizeof(this->b.pos));
}

Triangle::~Triangle()
{
}
