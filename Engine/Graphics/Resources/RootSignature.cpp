#include "..\..\Headers\Graphics\Resources\RootSignature.h"
#include "..\..\Headers\Graphics\d3dx12.h"
#include "..\..\Headers\Graphics\Error_Check.h"
#include <array>

RootSignature::RootSignature(Graphics* pGraphics, std::string code)
    :
    Code(code)
{

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(pGraphics->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }


    // Allow input layout and deny uneccessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(NULL, NULL, 0, nullptr, rootSignatureFlags);

    ID3DBlob* signature;
    ID3DBlob* error;
    Error_Check(
        D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error)
    );
    Error_Check(
        pGraphics->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))
    );

    if(signature)
        signature->Release();
    if(error)
        error->Release();

    signature = nullptr;
    error = nullptr;
}

HeapDescriptorArray::HeapDescriptorArray()
{
}

void HeapDescriptorArray::Bind(Graphics* pGraphics)
{
    auto pCommandList = pGraphics->GetCommandList();
    pCommandList->SetDescriptorHeaps(static_cast<UINT>(pHeaps.size()), pHeaps.data());
    for (size_t i = 0; i < Parameters.size(); i++)
        Parameters[i]->Bind(pCommandList);

}

CD3DX12_CPU_DESCRIPTOR_HANDLE HeapDescriptorArray::GetCPUHandle(UINT RootParam, UINT Range, UINT RangeIndex)
{
    return Parameters[RootParam]->Ranges[Range].GetCPUHandle(RangeIndex);
}

HeapDescriptorArray::~HeapDescriptorArray()
{
    for (auto& e : Parameters)
        delete e;
    for (auto& el : pHeaps)
        el->Release();
    pHeaps.clear();
    Parameters.clear();

}


RootSignature::RootSignature(Graphics* pGraphics, RS_Layout& Lay, std::string code)
    :
    Code(code)
{
    auto pDevice = pGraphics->GetDevice();
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    // Create root parameters.
    UINT Count = static_cast<UINT>(Lay.Params.size());
    std::vector<CD3DX12_ROOT_PARAMETER1> rParameters(Count);

    // Create ranges.
    std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE1>> pRanges;
       
    // Create bindable element for all heap stuff, it bind all data on heap in pipeline. 
    HeapArray = std::make_shared<HeapDescriptorArray>();

    // Create two heaps each for heap type;
    std::vector<HeapDescriptorArray::RootParameter*> pHeap_CBV_SHR_UAV;
    UINT Count_CBV_SHR_UAV = 0u;
    std::vector<HeapDescriptorArray::RootParameter*> pHeap_SAMPLERS;
    UINT Count_SAMPLERS = 0u;
        
    // First cycle for Parameters.
    for (UINT i = 0; i < Count; i++)
    {
        // Current pointer to heap.
        std::vector<HeapDescriptorArray::RootParameter*>* pCurr = nullptr;
        UINT* pCount = nullptr;

        switch (Lay[i].Ranges[0].Type)
        {
        case D3D12_DESCRIPTOR_RANGE_TYPE_CBV: case D3D12_DESCRIPTOR_RANGE_TYPE_SRV: case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
            pCurr = &pHeap_CBV_SHR_UAV;
            pCount = &Count_CBV_SHR_UAV;
            break;
        case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
            pCurr = &pHeap_SAMPLERS;
            pCount = &Count_SAMPLERS;
            break;
        }

        // Create parameters in HeapArray.
        switch (Lay[i].type)
        {
        case RS_Layout::Type::DescriptorTable:
            pCurr->push_back(new HeapDescriptorArray::DescriptorTable(i));
            break;
        //case RS_Layout::Type::Range:
        //    HeapArray->Parameters.push_back(HeapDescriptorArray::Range(i, Offset));
        //    break;
        //case RS_Layout::Type::Constant:
        //    HeapArray->Parameters.push_back(HeapDescriptorArray::Constant(i, Offset));
        //    break;
        }

        // Create range in vector.
        pRanges.emplace_back();

        // Second cycle for ranges.
        for (UINT j = 0; j < Lay[i].Ranges.size(); j++)
        {
            // Create ranges.
            RS_Layout::RootParameter::Range& r = Lay[i].Ranges[j];
            pRanges[i].emplace_back(r.Type, r.numDescriptors, r.ShaderRegister, r.RegisterSpace, r.Flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
           
            // Add range to our HeapArray and increase descriptors count.
            pCurr->back()->Ranges.emplace_back(r.Type, r.numDescriptors);
            *pCount += r.numDescriptors;
        }
               
        // Create root parameters.
        rParameters[i].InitAsDescriptorTable(static_cast<UINT>(pRanges[i].size()), pRanges[i].data(), Lay[i].Visibility);
    }

    // Allow input layout and deny uneccessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // Create descriptor for root signature.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc(Count, rParameters.data(), 0, nullptr, rootSignatureFlags);

    // Create versioned root signature.
    ID3DBlob* signature;
    ID3DBlob* error;
    Error_Check(
        D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error)
    );
    Error_Check(
        pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))
    );


    // Create two heap
    {
        // Initialize desc.
        D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
        Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        Desc.NumDescriptors = Count_CBV_SHR_UAV;
        std::vector<HeapDescriptorArray::RootParameter*>* pCurr = &pHeap_CBV_SHR_UAV;
        for (char i = 0; i < 2; i++)
        {
            // Create heap.
            ID3D12DescriptorHeap* pHeap;
            Error_Check(
                pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap))
            );
            // Add to our array.
            HeapArray->pHeaps.push_back(pHeap);

            // Get pointers.
            CD3DX12_GPU_DESCRIPTOR_HANDLE Ptr_GPU(pHeap->GetGPUDescriptorHandleForHeapStart());
            UINT Size = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);
            CD3DX12_CPU_DESCRIPTOR_HANDLE Ptr_CPU(pHeap->GetCPUDescriptorHandleForHeapStart());

            // Assign pointers to our array.
            for (UINT i = 0; i < pCurr->size(); i++)
            {
                (*pCurr)[i]->GPU_OffsetFromStart = Ptr_GPU;
                UINT Offset_GPU = 0u;
                for (UINT j = 0; j < (*pCurr)[i]->Ranges.size(); j++)
                {
                    (*pCurr)[i]->Ranges[j].CPU_Offset = Ptr_CPU;
                    Ptr_CPU.Offset((*pCurr)[i]->Ranges[j].NumDescriptors, Size);
                    Offset_GPU += (*pCurr)[i]->Ranges[j].Size = static_cast<UINT>(Ptr_CPU.ptr - (*pCurr)[i]->Ranges[j].CPU_Offset.ptr);
                }

                Ptr_GPU.Offset(1, Offset_GPU);
                (*pCurr)[i]->Size = static_cast<UINT>(Ptr_GPU.ptr - (*pCurr)[i]->GPU_OffsetFromStart.ptr);
            }

            // Set next heap.
            Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            Desc.NumDescriptors = Count_SAMPLERS;
            pCurr = &pHeap_SAMPLERS;
        }

        // Add our params to array.
        HeapArray->Parameters.insert(HeapArray->Parameters.begin(), pHeap_CBV_SHR_UAV.begin(), pHeap_CBV_SHR_UAV.end());
        HeapArray->Parameters.insert(HeapArray->Parameters.end(), pHeap_SAMPLERS.begin(), pHeap_SAMPLERS.end());

    }

    // Delete unnecessary data.
    if (signature)
        signature->Release();
    if (error)
        error->Release();

    signature = nullptr;
    error = nullptr;
}

void RootSignature::Bind(Graphics* pGraphics)
{
    pGraphics->GetCommandList()->SetGraphicsRootSignature(pRootSignature);
}

RootSignature::~RootSignature()
{
    pRootSignature->Release();
    pRootSignature = nullptr;
}

std::string RootSignature::GetCode()
{
    return Code;
}

std::shared_ptr<HeapDescriptorArray> RootSignature::GetHeapArray()
{
    return std::move(HeapArray);
}

RS_Layout::RS_Layout(D3D12_ROOT_SIGNATURE_FLAGS F)
    :
    Flags(F)
{
}

RS_Layout::RootParameter& RS_Layout::AddDescriptorTable(D3D12_SHADER_VISIBILITY V)
{
    Params.emplace_back(V, RS_Layout::Type::DescriptorTable);
    return Params.back();
}

RS_Layout::RootParameter& RS_Layout::operator[](UINT index)
{
    return Params[index];
}

std::string RS_Layout::GetCode()
{
    std::string code;
    for (size_t i = 0; i < Params.size(); i++)
    {
        code += '{' + std::to_string(static_cast<UINT>(Params[i].type)) + '[' + std::to_string(i) + "]:";
        code += Params[i].GetCode() + "}";
    }
    return code;
}

RS_Layout::RootParameter::Range::Range(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT N, UINT SR, UINT RS, D3D12_DESCRIPTOR_RANGE_FLAGS F)
    :
    Type(T),
    numDescriptors(N),
    ShaderRegister(SR),
    RegisterSpace(RS),
    Flags(F)
{}

std::string RS_Layout::RootParameter::Range::GetCode()
{
    std::string code;
    return code = std::to_string(Type) + ',' + std::to_string(numDescriptors) + ',' + std::to_string(ShaderRegister) + ',' + std::to_string(RegisterSpace) + ',' + std::to_string(Flags);
}

RS_Layout::RootParameter::RootParameter(D3D12_SHADER_VISIBILITY V, Type t)
    :
    Visibility(V),
    type(t)
{}

RS_Layout::RootParameter& RS_Layout::RootParameter::AddRange(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT numDescriptors, UINT ShaderRegister, UINT RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAGS Flags)
{
    Ranges.emplace_back(T, numDescriptors, ShaderRegister, RegisterSpace, Flags);
    return *this;
}

RS_Layout::RootParameter::Range& RS_Layout::RootParameter::operator[](UINT index)
{
    return Ranges[index];
}

std::string RS_Layout::RootParameter::GetCode()
{
    std::string code;
    for (size_t i = 0; i < Ranges.size(); i++)
    {
        code += "{Range" + '[' + std::to_string(i) + "]:";
        code += Ranges[i].GetCode() + "}";
    }
    return code;
}

HeapDescriptorArray::RootParameter::RootParameter(UINT Index)
    :
    Index(Index)
{}

void HeapDescriptorArray::RootParameter::Bind(ID3D12GraphicsCommandList * pCommandList)
{
    pCommandList->SetGraphicsRootDescriptorTable(Index, GPU_OffsetFromStart);
}

HeapDescriptorArray::RootParameter::Range::Range(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Nums)
    :
    Type(Type),
    NumDescriptors(Nums)
{}


CD3DX12_CPU_DESCRIPTOR_HANDLE HeapDescriptorArray::RootParameter::Range::GetCPUHandle(UINT Index)
{
    UINT Step = Size / NumDescriptors;
    CD3DX12_CPU_DESCRIPTOR_HANDLE Offset = CPU_Offset;
    return Offset.Offset(Index, Step);
}
