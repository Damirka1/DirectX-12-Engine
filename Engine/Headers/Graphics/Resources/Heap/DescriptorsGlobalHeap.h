#pragma once
#include "DescriptorHeapArray.h"

class Sampler;

class DescriptorsGlobalHeap
{
public:
	DescriptorsGlobalHeap();

	void Bind(Graphics* pGraphics);

	void IncrementDescCountForCbvSrvUav();

	void IncrementDescCoutForSamplers();

	void Initialize(Graphics* pGraphics);

	void SetResource(std::shared_ptr<Resource> pResource);

	void SetSampler(std::shared_ptr<Sampler> pSampler);

	// update for cbv srv uav heap
	bool IsNeedUpdateHeap0();

	void UpdateHeap0(Graphics* pGraphics);

	// update for sampler heap
	bool IsNeedUpdateHeap1();

	void UpdateHeap1(Graphics* pGraphics);

private:
	bool Initialized = false;

	// Descriptor heap for cbv srv uav
	DescriptorHeapArray DescHeapArr0;

	// Descriptor heap samplers
	DescriptorHeapArray DescHeapArr1;
};