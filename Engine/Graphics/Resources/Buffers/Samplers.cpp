#include "..\..\..\Headers\Graphics\Resources\Buffers\Samplers.h"

Sampler::Sampler(D3D12_SAMPLER_DESC* pDesc) noexcept
	:
	pDesc(*pDesc)
{
}

void Sampler::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->SetGraphicsRootDescriptorTable(Index, pGpuHandle);
}

void Sampler::Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
{
	pGraphics->GetDevice()->CreateSampler(&pDesc, pHandle);
	Initialized = true;
}

Sampler::~Sampler()
{
}
