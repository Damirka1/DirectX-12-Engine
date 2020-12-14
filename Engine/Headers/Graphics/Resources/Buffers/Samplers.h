#pragma once
#ifndef SAMPLERS_HEADER
#include "..\Bindable.h"

class Sampler : public Bindable
{
public:
	Sampler(D3D12_SAMPLER_DESC* pDesc) noexcept;
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle) override;
	~Sampler() override;
private:
	D3D12_SAMPLER_DESC pDesc;
};


#endif
