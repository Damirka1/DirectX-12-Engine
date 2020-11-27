#include "..\..\..\Headers\Graphics\Resources\Buffers\Samplers.h"

Sampler::Sampler(D3D12_SAMPLER_DESC* pDesc)
	:
	pDesc(*pDesc)
{
}

void Sampler::Bind(Graphics* pGraphics)
{
	// Do nothing. Bind from heap.
}

void Sampler::Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
{
	if (!Initialized)
	{
		pGraphics->GetDevice()->CreateSampler(&pDesc, pHandle);
		Initialized = true;
	}
}

Sampler::~Sampler()
{
}
