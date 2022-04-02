#include "..\..\Headers\Graphics\Resources\Heap.h"
#include "..\..\Headers\Graphics\Error_Check.h"
#include "..\..\Headers\Graphics\Resources\RootSignature.h"

HeapDescriptorArray::HeapDescriptorArray()
{
}

void HeapDescriptorArray::Initialize(RS_Layout& Lay) noexcept
{
    // First cycle for Parameters.
    for (UINT i = 0; i < Lay.Params.size(); i++)
    {
        switch (Lay[i].type)
        {
        case RS_Layout::Type::DescriptorTable:
            Parameters.push_back(new HeapDescriptorArray::DescriptorTable(i));
            break;
            //case RS_Layout::Type::Range:
            //    HeapArray->Parameters.push_back(HeapDescriptorArray::Range(i, Offset));
            //    break;
            //case RS_Layout::Type::Constant:
            //    HeapArray->Parameters.push_back(HeapDescriptorArray::Constant(i, Offset));
            //    break;
        }

        UINT ParamIndex = static_cast<UINT>(Parameters.size()) - 1u;
        // Second cycle for ranges.
        for (UINT j = 0; j < Lay[i].Ranges.size(); j++)
        {
            RS_Layout::RootParameter::Range& r = Lay[i].Ranges[j];
            Parameters[ParamIndex]->Ranges.emplace_back(r.Type, r.numDescriptors);
        }
    }
}

void HeapDescriptorArray::InitializePointers(Graphics* pGraphics, CD3DX12_CPU_DESCRIPTOR_HANDLE& pCPU, CD3DX12_GPU_DESCRIPTOR_HANDLE& pGPU, CD3DX12_CPU_DESCRIPTOR_HANDLE& pCPUForSamplers, CD3DX12_GPU_DESCRIPTOR_HANDLE& pGPUForSamplers) noexcept
{
    auto pDevice = pGraphics->GetDevice();

    for (auto& el : Parameters)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE* pCPUCurr = nullptr;
        CD3DX12_GPU_DESCRIPTOR_HANDLE* pGPUCurr = nullptr;
        UINT IncrementSize = 0u;
        UINT Offset = 0u;

        switch (el->Ranges.back().Type)
        {
        case D3D12_DESCRIPTOR_RANGE_TYPE_CBV: case D3D12_DESCRIPTOR_RANGE_TYPE_SRV: case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
            pCPUCurr = &pCPU;
            pGPUCurr = &pGPU;
            IncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            break;
        case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
            pCPUCurr = &pCPUForSamplers;
            pGPUCurr = &pGPUForSamplers;
            IncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            break;
        }

        el->GPU_OffsetFromStart = *pGPUCurr;

        for (auto& Ranges : el->Ranges)
        {
            Ranges.CPU_Offset = *pCPUCurr;
            pCPUCurr->Offset(Ranges.NumDescriptors, IncrementSize);
            Offset += Ranges.Size = static_cast<UINT>(pCPUCurr->ptr - Ranges.CPU_Offset.ptr);
        }
        pGPUCurr->Offset(Offset);
    }
}

void HeapDescriptorArray::Bind(Graphics* pGraphics)
{
    auto pCommandList = pGraphics->GetCommandList();
    for (size_t i = 0; i < Parameters.size(); i++)
        Parameters[i]->Bind(pCommandList);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE HeapDescriptorArray::GetCPUHandle(UINT RootParam, UINT Range, UINT RangeIndex) noexcept
{
    return Parameters[RootParam]->Ranges[Range].GetCPUHandle(RangeIndex);
}

HeapDescriptorArray::~HeapDescriptorArray()
{
    for (auto& e : Parameters)
        delete e;
    Parameters.clear();
}


HeapDescriptorArray::RootParameter::RootParameter(UINT Index) noexcept
    :
    Index(Index)
{}

void HeapDescriptorArray::RootParameter::Bind(ID3D12GraphicsCommandList* pCommandList)
{
    pCommandList->SetGraphicsRootDescriptorTable(Index, GPU_OffsetFromStart);
}

HeapDescriptorArray::RootParameter::Range::Range(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Nums) noexcept
    :
    Type(Type),
    NumDescriptors(Nums)
{}


CD3DX12_CPU_DESCRIPTOR_HANDLE HeapDescriptorArray::RootParameter::Range::GetCPUHandle(UINT Index) noexcept
{
    UINT Step = Size / NumDescriptors;
    CD3DX12_CPU_DESCRIPTOR_HANDLE Offset = CPU_Offset;
    return Offset.Offset(Index, Step);
}

void GlobalHeap::Add_CBV_SHR_UAV_Desc(UINT Count) noexcept
{
    CBV_SHR_UAV_Desc_count += Count;
    NeedUpdate = true;
}

void GlobalHeap::Add_Samplers_Desc(UINT Count) noexcept
{
    SAMPLERS_Desc_count += Count;
    NeedUpdate = true;
}

void GlobalHeap::Initialize(Graphics* pGraphics)
{
    auto pDevice = pGraphics->GetDevice();

    // Initialize desc.
    D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
    Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    Desc.NumDescriptors = CBV_SHR_UAV_Desc_count;
    for (char i = 0; i < 2; i++)
    {
        if (Desc.NumDescriptors == 0)
            continue;
        // Create heap.
        ID3D12DescriptorHeap* pHeap;
        Error_Check(
            pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap))
        );
        // Add to our array.
        pHeaps[i] = pHeap;

        // Set next heap.
        Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        Desc.NumDescriptors = SAMPLERS_Desc_count;
    }

    if (pHeaps[0])
    {
        HeapCBVInitialized = true;
        ActiveHeaps++;
    }
    if (pHeaps[1])
    {
        HeapSamplersInitialized = true;
        ActiveHeaps++;
    }

    Initialized = true;
    NeedUpdate = false;
}

D3D12_CPU_DESCRIPTOR_HANDLE GlobalHeap::GetCPUStartPtr() noexcept
{
    return pHeaps[0]->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE GlobalHeap::GetGPUStartPtr() noexcept
{
    return pHeaps[0]->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE GlobalHeap::GetCPUStartPtrForSAMPLERS() noexcept
{
    return pHeaps[1]->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE GlobalHeap::GetGPUStartPtrForSAMPLERS() noexcept
{
    return pHeaps[1]->GetGPUDescriptorHandleForHeapStart();
}

bool GlobalHeap::isHeapForCbvInitialized() noexcept
{
    return HeapCBVInitialized;
}

bool GlobalHeap::isHeapForSamplersInitialized() noexcept
{
    return HeapSamplersInitialized;
}

void GlobalHeap::Bind(Graphics* pGraphics)
{
    pGraphics->GetCommandList()->SetDescriptorHeaps(ActiveHeaps, pHeaps.data());
}

void GlobalHeap::Clear(Graphics* pGraphics)
{
    if (Initialized)
    {
        pHeaps[0]->Release();
        pHeaps[1]->Release();
        Initialized = false;
        ActiveHeaps = 0;
    }
}

bool GlobalHeap::IsNeedUpdate()
{
    return NeedUpdate;
}

GlobalHeap::~GlobalHeap()
{
    for (auto& el : pHeaps)
    {
        if(el)
            el->Release();
    }
}

HeapDescriptorArray::DescriptorTable::DescriptorTable(UINT Index) noexcept
    :
    RootParameter(Index)
{}

void HeapDescriptorArray::DescriptorTable::Bind(ID3D12GraphicsCommandList* pCommandList)
{
    pCommandList->SetGraphicsRootDescriptorTable(Index, GPU_OffsetFromStart);
}
