#include "..\..\Headers\Graphics\Resources\RootSignature.h"
#include "..\..\Headers\Graphics\d3dx12.h"
#include "..\..\Headers\Graphics\Error_Check.h"

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

RootSignature::RootSignature(Graphics* pGraphics, RS_Layout& Lay, std::string code)
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


    // Create root parameters.
    UINT Count = static_cast<UINT>(Lay.DescriptorTables.size());
    CD3DX12_ROOT_PARAMETER1* rParameters = new CD3DX12_ROOT_PARAMETER1[Count];
    
    for (UINT i = 0; i < Count; i++)
    {
        // Create ranges.
        UINT RCount = static_cast<UINT>(Lay[i].Ranges.size());
        CD3DX12_DESCRIPTOR_RANGE1* ranges = new CD3DX12_DESCRIPTOR_RANGE1[RCount];

        for (UINT j = 0; j < RCount; j++)
        {
            // Init ranges.
            RS_Layout::DescriptorTable::Range& r = Lay[i].Ranges[j];
            ranges[j].Init(r.Type, r.numDescriptors, r.ShaderRegister, r.RegisterSpace, r.Flags);
        }

        // Init root parameters.
        rParameters[i].InitAsDescriptorTable(RCount, ranges, Lay[i].Visibility);

        delete[] ranges;
    }
    
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(Count, rParameters, 0, nullptr, Lay.Flags);

    delete[] rParameters;


    ID3DBlob* signature;
    ID3DBlob* error;
    Error_Check(
        D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error)
    );
    Error_Check(
        pGraphics->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))
    );

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

RS_Layout::RS_Layout(D3D12_ROOT_SIGNATURE_FLAGS F)
    :
    Flags(F)
{
}

RS_Layout::DescriptorTable& RS_Layout::AddDescriptorTable(D3D12_SHADER_VISIBILITY V)
{
    DescriptorTables.emplace_back(V);
    return DescriptorTables.back();
}

RS_Layout::DescriptorTable& RS_Layout::operator[](UINT index)
{
    return DescriptorTables[index];
}

std::string RS_Layout::GetCode()
{
    std::string code;
    for (size_t i = 0; i < DescriptorTables.size(); i++)
    {
        code += "{Descriptor Table" + '[' + std::to_string(i) + "]:";
        code += DescriptorTables[i].GetCode() + "}";
    }
    return code;
}

RS_Layout::DescriptorTable::Range::Range(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT N, UINT SR, UINT RS, D3D12_DESCRIPTOR_RANGE_FLAGS F)
    :
    Type(T),
    numDescriptors(N),
    ShaderRegister(SR),
    RegisterSpace(RS),
    Flags(F)
{}

std::string RS_Layout::DescriptorTable::Range::GetCode()
{
    std::string code;
    return code = std::to_string(Type) + ',' + std::to_string(numDescriptors) + ',' + std::to_string(ShaderRegister) + ',' + std::to_string(RegisterSpace) + ',' + std::to_string(Flags);
}

RS_Layout::DescriptorTable::DescriptorTable(D3D12_SHADER_VISIBILITY V)
    :
    Visibility(V)
{}

RS_Layout::DescriptorTable& RS_Layout::DescriptorTable::AddRange(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT numDescriptors, UINT ShaderRegister, UINT RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAGS Flags)
{
    Ranges.emplace_back(T, numDescriptors, ShaderRegister, RegisterSpace, Flags);
    return *this;
}

RS_Layout::DescriptorTable::Range& RS_Layout::DescriptorTable::operator[](UINT index)
{
    return Ranges[index];
}

std::string RS_Layout::DescriptorTable::GetCode()
{
    std::string code;
    for (size_t i = 0; i < Ranges.size(); i++)
    {
        code += "{Range" + '[' + std::to_string(i) + "]:";
        code += Ranges[i].GetCode() + "}";
    }
    return code;
}
