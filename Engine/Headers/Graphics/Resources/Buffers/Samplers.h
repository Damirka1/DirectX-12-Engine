#pragma once
#ifndef SAMPLERS_HEADER
#include "..\Resource.h"

class Sampler : public Resource
{
public:
	Sampler(D3D12_SAMPLER_DESC* pDesc) noexcept;
	void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle) override;
	~Sampler() override;
private:
	D3D12_SAMPLER_DESC pDesc;
};


#endif
