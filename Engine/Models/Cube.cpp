#include "Cube.h"

Cube::Cube(std::string TexturePath, ResourceManager* pRM)
{

	struct VB
	{
		float x, y, z;
	};

	VB vb[] = {
		{-1.0f,  1.0f, -1.0f},
		{ 1.0f,  1.0f, -1.0f},
		{ 1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
							
		{ 1.0f,  1.0f,	1.0f},
		{-1.0f,  1.0f,	1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f},
	};



	VertexLayout Lay;
	Lay.AddElement("Position", DXGI_FORMAT_R32G32B32_FLOAT);

	SetVertexbuffer(pRM->CreateVertexBuffer(this, reinterpret_cast<void*>(vb), sizeof(VB), sizeof(vb), Lay));

	PSO_Layout pLay(1);
	pLay.DepthState(true);
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("..\\Engine\\Shaders\\VertexShader.cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("..\\Engine\\Shaders\\PixelShader.cso"));
	std::string PSO_key = pRM->CreatePSO(pLay, &Lay);


	RS_Layout RsLay;
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL).
		AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1);

	std::string RS_key = pRM->CreateRootSignature(PSO_key, RsLay, this);

	b.Pos = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity() * DirectX::XMMatrixTranslation(0.0f, 0.0f, 5.0f));
	b.Projection = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), 4.0f/3.0f, 0.01f, 50.0f));

	{
		const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const auto lookVector = forwardBaseVector;
		const auto camPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);;
		const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
		b.View = DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}

	pConstBuffer = pRM->CreateConstBuffer(this, &b, sizeof(b), 0, 0, 0);
	pConstBuffer->SetKeys(PSO_key, RS_key);

	FaceColor[0] = { 1.0f, 0.0f, 1.0f, 1.0f };
	FaceColor[1] = { 1.0f, 0.0f, 0.0f, 1.0f };
	FaceColor[2] = { 0.0f, 1.0f, 0.0f, 1.0f };
	FaceColor[3] = { 0.0f, 0.0f, 1.0f, 1.0f };
	FaceColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	FaceColor[5] = { 0.0f, 1.0f, 1.0f, 1.0f };
	

	pConstBufferColors = pRM->CreateConstBuffer(this, &FaceColor, sizeof(FaceColor), 0, 0, 1);
	pConstBufferColors->SetKeys(PSO_key, RS_key);

	pTexture = pRM->CreateTexture2D(this, TexturePath, 0, 1, 0);
	pTexture->SetKeys(PSO_key, RS_key);
	pSampler = pRM->CreateDefaultSampler(this, 1, 0, 0);
	pSampler->SetKeys(PSO_key, RS_key);


	std::vector<unsigned int> indecies = { 
				0,1,2,
				2,3,0,

				3,2,7,
				7,6,3,

				1,4,7,
				7,2,1,

				4,5,6,
				6,7,4,

				0,5,4,
				4,1,0,

				0,3,6,
				6,5,0
	};
	SetIndexBuffer(pRM->CreateIndexBuffer(this, indecies));
}

void Cube::Update(float* pArray, int Count)
{
	static float angle = 0.0f;
	angle += 1.0f;
	
	b.Pos = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationAxis({ -1.0f, 1.0f, 1.0f }, DirectX::XMConvertToRadians(angle)) * DirectX::XMMatrixRotationRollPitchYaw(pArray[3], pArray[4], pArray[5]) * DirectX::XMMatrixTranslation(pArray[0], pArray[1], 5.0f + pArray[2]));
	pConstBuffer->Update(&b, sizeof(b));
}

Cube::~Cube()
{
}
