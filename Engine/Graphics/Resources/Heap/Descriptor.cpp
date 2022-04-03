#include "../../Headers/Graphics/Resources/Heap/Descriptor.h"
#include "../../Headers/Graphics/Resources/Resource.h"

bool Descriptor::SetResource(std::shared_ptr<Resource> pResource)
{
	if (!InUse)
	{
		this->pResource = std::move(pResource);
		this->pResource->SetDescriptor(this);
		InUse = true;
		return true;
	}
	return false;
}

void Descriptor::RemoveResource()
{
	if(pResource)
		pResource.reset();
	InUse = false;
}

D3D12_GPU_DESCRIPTOR_HANDLE Descriptor::GetGpuHandle()
{
	return pGpuHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE Descriptor::GetCpuHandle()
{
	return pCpuHandle;
}

void Descriptor::Initialize(D3D12_GPU_DESCRIPTOR_HANDLE& pGpu, D3D12_CPU_DESCRIPTOR_HANDLE& pCpu)
{
	pGpuHandle = pGpu;
	pCpuHandle = pCpu;

	Initialized = true;
}

Descriptor::~Descriptor()
{
	if(pResource)
		pResource->RemoveDescriptor();
}
