#include "Cube.h"
#include <random>

Cube::Cube(ResourceManager* pRM, DirectX::XMFLOAT3 Pos) noexcept
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

	SetVertexAndIndexBuffers(pRM->CreateVertexBuffer(this, reinterpret_cast<void*>(vb), sizeof(VB), sizeof(vb), Lay, 24),
							 pRM->CreateIndexBuffer(this, &indecies));


	PSO_Layout pLay(1);
	pLay.DepthState(true);
	pLay.SetShader(PSO_Layout::Shader::Vertex, std::string("Shaders\\VertexShader.cso"));
	pLay.SetShader(PSO_Layout::Shader::Pixel, std::string("Shaders\\PixelShader.cso"));
	
	RS_Layout RsLay;
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);
	RsLay.AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL)
		.AddRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	std::shared_ptr<PipelineStateObject> pPSO = pRM->CreatePipelineStateObject(pLay, Lay);

	SetPipelineStateObjectAndRootSignature(pPSO, pRM->CreateRootSignature(this, pPSO->GetKey(), RsLay));

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

		std::uniform_real_distribution<float> ColorGen(0.0, 1.0);

		Color = { ColorGen(gen), ColorGen(gen), ColorGen(gen), 1.0f };

		pConstBufferColors = pRM->CreateConstBuffer(this, &Color, sizeof(Color), 1, 0, 0);

		Drawable::Pos = Pos;
		Transformation = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationAxis({ ax,ay,az }, DirectX::XMConvertToRadians(angle)) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * *pCamera.View * *pCamera.Projection);
		pConstBuffer = pRM->CreateConstBuffer(this, &Transformation, sizeof(Transformation), 0, 0, 0);
	}
}

void Cube::Update()
{
	Transformation = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationAxis({ ax,ay,az }, DirectX::XMConvertToRadians(angle)) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Pos)) * *pCamera.View * *pCamera.Projection);
	pConstBuffer->Update(&Transformation, sizeof(Transformation));

	angle += dAngle;
}

Cube::~Cube()
{
}
