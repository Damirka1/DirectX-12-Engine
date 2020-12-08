#include "Cube.h"
#include <random>

Cube::Cube(ResourceManager* pRM, DirectX::XMFLOAT3 Pos)
	:
	Pos(Pos)
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
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	std::string RS_key = pRM->CreateRootSignature(PSO_key, RsLay, this);

	b.Pos = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 5.0f));
	b.Projection = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), 4.0f/3.0f, 0.01f, 150.0f));

	{
		const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const auto lookVector = forwardBaseVector;
		const auto camPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);;
		const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
		b.View = DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}

	pConstBuffer = pRM->CreateConstBuffer(this, &b, sizeof(b), 0, 0, 0);
	pConstBuffer->SetKeys(PSO_key, RS_key);

	

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

	// Random numbers for rotation.
	{
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		std::minstd_rand gen(rd());
		std::uniform_real_distribution<float> dis(-1.0, 1.0);
		std::uniform_real_distribution<float> disAngle(-5.0, 5.0);

		ax = dis(gen);
		ay = dis(gen);
		az = dis(gen);
		dAngle = disAngle(gen);

		std::uniform_real_distribution<float> Color(0.0, 1.0);

		FaceColor[0] = { Color(gen), Color(gen), Color(gen), 1.0f };
		FaceColor[1] = { Color(gen), Color(gen), Color(gen), 1.0f };
		FaceColor[2] = { Color(gen), Color(gen), Color(gen), 1.0f };
		FaceColor[3] = { Color(gen), Color(gen), Color(gen), 1.0f };
		FaceColor[4] = { Color(gen), Color(gen), Color(gen), 1.0f };
		FaceColor[5] = { Color(gen), Color(gen), Color(gen), 1.0f };


		pConstBufferColors = pRM->CreateConstBuffer(this, &FaceColor, sizeof(FaceColor), 1, 0, 0);
		pConstBufferColors->SetKeys(PSO_key, RS_key);
	}
}

void Cube::Update(DirectX::XMFLOAT3 Translation)
{
	b.Pos = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationAxis({ ax,ay,az }, DirectX::XMConvertToRadians(angle)) * DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Pos), DirectX::XMLoadFloat3(&Translation))));
	pConstBuffer->Update(&b, sizeof(b));

	angle += dAngle;
}

Cube::~Cube()
{
}
