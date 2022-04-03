#include "../../Headers/Graphics/Resources/Heap/DescriptorHeapArray.h"
#include "../../Headers/Graphics.h"

DescriptorHeapArray::DescriptorHeapArray(D3D12_DESCRIPTOR_HEAP_TYPE Type)
{
	Desc.NumDescriptors = 1;
	Desc.Type = Type;
	Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
}

DescriptorHeapArray::~DescriptorHeapArray()
{
	pDescriptorsHeap->Release();
}

void DescriptorHeapArray::IncrementDescriptorsCount()
{
	Desc.NumDescriptors += 1;
	NeedUpdate = true;
}

void DescriptorHeapArray::DecrementDescriptorsCount()
{
	Desc.NumDescriptors -= 1;
	NeedUpdate = true;
}

ID3D12DescriptorHeap* DescriptorHeapArray::GetPtr()
{
	return pDescriptorsHeap;
}

void DescriptorHeapArray::Initialize(Graphics* pGraphics)
{
	ID3D12Device9* pDevice = pGraphics->GetDevice();

	Error_Check(
		pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pDescriptorsHeap))
	);

	InitDescCount = Desc.NumDescriptors;

	Descriptors.resize(InitDescCount);

	UINT OffsetSize = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);

	CD3DX12_GPU_DESCRIPTOR_HANDLE pGpuStart(pDescriptorsHeap->GetGPUDescriptorHandleForHeapStart());
	CD3DX12_CPU_DESCRIPTOR_HANDLE pCpuStart(pDescriptorsHeap->GetCPUDescriptorHandleForHeapStart());

	for (size_t i = 0; i < InitDescCount; i++)
	{
		std::unique_ptr<Descriptor> dh = std::make_unique<Descriptor>();

		dh->Initialize(pGpuStart, pCpuStart);

		pGpuStart.Offset(OffsetSize);
		pCpuStart.Offset(OffsetSize);

		Descriptors[i] = std::move(dh);
	}
	Initialized = true;
	NeedUpdate = false;
}

void DescriptorHeapArray::SetResource(std::shared_ptr<Resource> pResource)
{
	for (auto& desc : Descriptors)
	{
		if (desc->SetResource(pResource))
			break;
	}
}

bool DescriptorHeapArray::IsNeedUpdate()
{
	return NeedUpdate;
}

void DescriptorHeapArray::Update(Graphics* pGraphics)
{
	/*ID3D12DescriptorHeap* pTempHeap;

	ID3D12Device9* pDevice = pGraphics->GetDevice();

	Error_Check(
		pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pTempHeap))
	);

	pDevice->CopyDescriptorsSimple(InitDescCount, pTempHeap->GetCPUDescriptorHandleForHeapStart(), pDescriptorsHeap->GetCPUDescriptorHandleForHeapStart(), Desc.Type);

	InitDescCount = Desc.NumDescriptors;

	Descriptors.resize(InitDescCount);

	UINT OffsetSize = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);

	pDescriptorsHeap->Release();
	pDescriptorsHeap = pTempHeap;

	CD3DX12_GPU_DESCRIPTOR_HANDLE pGpuStart(pDescriptorsHeap->GetGPUDescriptorHandleForHeapStart());
	CD3DX12_CPU_DESCRIPTOR_HANDLE pCpuStart(pDescriptorsHeap->GetCPUDescriptorHandleForHeapStart());

	for (size_t i = 0; i < InitDescCount; i++)
	{
		std::unique_ptr<Descriptor> dh = std::make_unique<Descriptor>();

		dh->Initialize(pGpuStart, pCpuStart);

		pGpuStart.Offset(OffsetSize);
		pCpuStart.Offset(OffsetSize);

		Descriptors[i] = std::move(dh);
	}

	NeedUpdate = false;*/

	pDescriptorsHeap->Release();
	pDescriptorsHeap = nullptr;
	Descriptors.clear();

	Initialize(pGraphics);
}
