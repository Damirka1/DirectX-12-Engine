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
    for (size_t i = 0; i < pHeaps.size(); i++)
        pHeaps[i]->Release();
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
        
    // First cycle for Parameters.
    for (UINT i = 0; i < Count; i++)
    {
        // Create parameters in HeapArray.
        switch (Lay[i].type)
        {
        case RS_Layout::Type::DescriptorTable:
            HeapArray->Parameters.push_back(new HeapDescriptorArray::DescriptorTable(i));
            break;
        //case RS_Layout::Type::Range:
        //    HeapArray->Parameters.push_back(HeapDescriptorArray::Range(i, Offset));
        //    break;
        //case RS_Layout::Type::Constant:
        //    HeapArray->Parameters.push_back(HeapDescriptorArray::Constant(i, Offset));
        //    break;
        }

        D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
        Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      
        switch (Lay[i].Ranges[0].Type)
        {
        case D3D12_DESCRIPTOR_RANGE_TYPE_CBV: case D3D12_DESCRIPTOR_RANGE_TYPE_SRV: case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
            Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            break;
        case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
            Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            break;
        }

        pRanges.emplace_back();

        // Second cycle for ranges.
        for (UINT j = 0; j < Lay[i].Ranges.size(); j++)
        {
            // Create ranges.
            RS_Layout::RootParameter::Range& r = Lay[i].Ranges[j];
            pRanges[i].emplace_back(r.Type, r.numDescriptors, r.ShaderRegister, r.RegisterSpace, r.Flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
           
            // Add range to our HeapArray.
            HeapArray->Parameters[i]->Ranges.emplace_back(r.Type, r.numDescriptors);

            // Increment num descriptors and offset.
            Desc.NumDescriptors += r.numDescriptors;
        }
               
        // Create root parameters.
        rParameters[i].InitAsDescriptorTable(static_cast<UINT>(pRanges[i].size()), pRanges[i].data(), Lay[i].Visibility);

        ID3D12DescriptorHeap* pHeap;
        Error_Check(
            pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap))
        );
        HeapArray->pHeaps.push_back(pHeap);

        HeapArray->Parameters[i]->GPU_OffsetFromStart = pHeap->GetGPUDescriptorHandleForHeapStart();
        HeapArray->Parameters[i]->SetPointers(pDevice, Desc.Type, pHeap->GetCPUDescriptorHandleForHeapStart());

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

void HeapDescriptorArray::RootParameter::SetPointers(ID3D12Device8* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
{
    UINT Size = pDevice->GetDescriptorHandleIncrementSize(Type);
    CD3DX12_CPU_DESCRIPTOR_HANDLE It (Handle);

    this->Size = 0;
    for (UINT i = 0; i < Ranges.size(); i++)
    {
        Ranges[i].Size = static_cast<UINT>(It.ptr);
        Ranges[i].CPU_Offset = It;
        It.Offset(Ranges[i].NumDescriptors, Size);
        this->Size += Ranges[i].Size = static_cast<UINT>(It.ptr) - Ranges[i].Size;
    }
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
