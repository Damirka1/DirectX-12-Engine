#pragma once
#ifndef SAMPLERS_HEADER
#define SAMPLERS_HEADER
#include "..\Resource.h"

class Sampler : public Resource
{
public:
	Sampler(D3D12_SAMPLER_DESC* pDesc) noexcept;
	void Bind(Graphics* pGraphics);
	void Initialize(Graphics* pGraphics) override;
	~Sampler() override;
private:
	D3D12_SAMPLER_DESC pDesc;
};


#endif
