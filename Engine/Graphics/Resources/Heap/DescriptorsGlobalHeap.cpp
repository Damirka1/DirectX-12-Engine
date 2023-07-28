#include "../../Headers/Graphics/Resources/Heap/DescriptorsGlobalHeap.h"
#include "../../Headers/Graphics/Resources/Buffers/Samplers.h"

DescriptorsGlobalHeap::DescriptorsGlobalHeap()
	:
	DescHeapArr0(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
	DescHeapArr1(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
{

}

void DescriptorsGlobalHeap::Bind(Graphics* pGraphics)
{
	ID3D12DescriptorHeap* ppHeaps[] = { DescHeapArr0.GetPtr(), DescHeapArr1.GetPtr() };
	pGraphics->GetCommandList()->SetDescriptorHeaps(2, ppHeaps);
}

void DescriptorsGlobalHeap::IncrementDescCountForCbvSrvUav()
{
	DescHeapArr0.IncrementDescriptorsCount();
}

void DescriptorsGlobalHeap::IncrementDescCountForSamplers()
{
	DescHeapArr1.IncrementDescriptorsCount();
}

void DescriptorsGlobalHeap::Initialize(Graphics* pGraphics)
{
	if (!Initialized)
	{
		DescHeapArr0.Initialize(pGraphics);
		DescHeapArr1.Initialize(pGraphics);
		Initialized = true;
	}
}

void DescriptorsGlobalHeap::SetResource(std::shared_ptr<Resource> pResource)
{
	DescHeapArr0.SetResource(pResource);
}

void DescriptorsGlobalHeap::SetSampler(std::shared_ptr<Sampler> pSampler)
{
	DescHeapArr1.SetResource(pSampler);
}

// update for cbv srv uav heap
bool DescriptorsGlobalHeap::IsNeedUpdateHeap0()
{
	return DescHeapArr0.IsNeedUpdate();
}

void DescriptorsGlobalHeap::UpdateHeap0(Graphics* pGraphics)
{
	DescHeapArr0.Update(pGraphics);
}

// update for sampler heap
bool DescriptorsGlobalHeap::IsNeedUpdateHeap1()
{
	return DescHeapArr1.IsNeedUpdate();
}

void DescriptorsGlobalHeap::UpdateHeap1(Graphics* pGraphics)
{
	DescHeapArr1.Update(pGraphics);
}
