#include "..\..\..\Headers\Graphics\Resources\Buffers\Samplers.h"

Sampler::Sampler(D3D12_SAMPLER_DESC* pDesc) noexcept
	:
	pDesc(*pDesc)
{
	Name = "Sampler";
}

void Sampler::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->SetGraphicsRootDescriptorTable(Index, pDescriptor->GetGpuHandle());
}

void Sampler::Initialize(Graphics* pGraphics)
{
	pGraphics->GetDevice()->CreateSampler(&pDesc, pDescriptor->GetCpuHandle());
	Initialized = true;
}

Sampler::~Sampler()
{
}
