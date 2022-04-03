//#include "..\..\Headers\Graphics\Resources\Heap.h"
//#include "..\..\Headers\Graphics\Error_Check.h"
//#include "..\..\Headers\Graphics\Resources\RootSignature.h"
//
//void GlobalHeap::Add_CBV_SHR_UAV_Desc(UINT Count) noexcept
//{
//    CBV_SHR_UAV_Desc_count += Count;
//    NeedUpdate = true;
//}
//
//void GlobalHeap::Add_Samplers_Desc(UINT Count) noexcept
//{
//    SAMPLERS_Desc_count += Count;
//    NeedUpdate = true;
//}
//
//void GlobalHeap::Initialize(Graphics* pGraphics)
//{
//    auto pDevice = pGraphics->GetDevice();
//
//    // Initialize desc.
//    D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
//    Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//    Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//    Desc.NumDescriptors = CBV_SHR_UAV_Desc_count;
//    for (char i = 0; i < 2; i++)
//    {
//        if (Desc.NumDescriptors == 0)
//            continue;
//        // Create heap.
//        ID3D12DescriptorHeap* pHeap;
//        Error_Check(
//            pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap))
//        );
//        // Add to our array.
//        pHeaps[i] = pHeap;
//
//        // Set next heap.
//        Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
//        Desc.NumDescriptors = SAMPLERS_Desc_count;
//    }
//
//    if (pHeaps[0])
//    {
//        HeapCBVInitialized = true;
//        ActiveHeaps++;
//    }
//    if (pHeaps[1])
//    {
//        HeapSamplersInitialized = true;
//        ActiveHeaps++;
//    }
//
//    Initialized = true;
//    NeedUpdate = false;
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE GlobalHeap::GetCPUStartPtr() noexcept
//{
//    return pHeaps[0]->GetCPUDescriptorHandleForHeapStart();
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE GlobalHeap::GetGPUStartPtr() noexcept
//{
//    return pHeaps[0]->GetGPUDescriptorHandleForHeapStart();
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE GlobalHeap::GetCPUStartPtrForSAMPLERS() noexcept
//{
//    return pHeaps[1]->GetCPUDescriptorHandleForHeapStart();
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE GlobalHeap::GetGPUStartPtrForSAMPLERS() noexcept
//{
//    return pHeaps[1]->GetGPUDescriptorHandleForHeapStart();
//}
//
//bool GlobalHeap::isHeapForCbvInitialized() noexcept
//{
//    return HeapCBVInitialized;
//}
//
//bool GlobalHeap::isHeapForSamplersInitialized() noexcept
//{
//    return HeapSamplersInitialized;
//}
//
//void GlobalHeap::Bind(Graphics* pGraphics)
//{
//    pGraphics->GetCommandList()->SetDescriptorHeaps(ActiveHeaps, pHeaps.data());
//}
//
//void GlobalHeap::Clear(Graphics* pGraphics)
//{
//    if (Initialized)
//    {
//        pHeaps[0]->Release();
//        pHeaps[1]->Release();
//        Initialized = false;
//        ActiveHeaps = 0;
//    }
//}
//
//bool GlobalHeap::IsNeedUpdate()
//{
//    return NeedUpdate;
//}
//
//GlobalHeap::~GlobalHeap()
//{
//    for (auto& el : pHeaps)
//    {
//        if(el)
//            el->Release();
//    }
//}
