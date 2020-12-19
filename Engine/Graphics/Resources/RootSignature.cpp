#include "..\..\Headers\Graphics\Resources\RootSignature.h"
#include "..\..\Headers\Graphics\d3dx12.h"
#include "..\..\Headers\Graphics\Error_Check.h"
#include "..\..\Headers\Graphics\Resources\Heap.h"
#include <array>

RootSignature::RootSignature(RS_Layout& Lay) noexcept
    :
    Lay(Lay)
{}

void RootSignature::Initialize(Graphics* pGraphics)
{
    if (Initialized)
        return;

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


    // First cycle for Parameters.
    for (UINT i = 0; i < Count; i++)
    {
        // Create vector for ranges.
        pRanges.emplace_back();

        // Second cycle for ranges.
        for (UINT j = 0; j < Lay[i].Ranges.size(); j++)
        {
            // Create range.
            RS_Layout::RootParameter::Range& r = Lay[i].Ranges[j];
            pRanges[i].emplace_back(r.Type, r.numDescriptors, r.ShaderRegister, r.RegisterSpace, r.Flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

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

    // Delete unnecessary data.
    if (signature)
        signature->Release();
    if (error)
        error->Release();

    signature = nullptr;
    error = nullptr;

    Initialized = true;
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

RS_Layout::RS_Layout(D3D12_ROOT_SIGNATURE_FLAGS F) noexcept
    :
    Flags(F)
{
}

RS_Layout::RootParameter& RS_Layout::AddDescriptorTable(D3D12_SHADER_VISIBILITY V) noexcept
{
    Params.emplace_back(V, RS_Layout::Type::DescriptorTable);
    return Params.back();
}

RS_Layout::RootParameter& RS_Layout::operator[](UINT index) noexcept
{
    return Params[index];
}

std::string RS_Layout::GetCode() noexcept
{
    std::string code;
    for (size_t i = 0; i < Params.size(); i++)
    {
        code += '{' + std::to_string(static_cast<UINT>(Params[i].type)) + '[' + std::to_string(i) + "]:";
        code += Params[i].GetCode() + "}";
    }
    return code;
}

RS_Layout::RootParameter::Range::Range(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT N, UINT SR, UINT RS, D3D12_DESCRIPTOR_RANGE_FLAGS F) noexcept
    :
    Type(T),
    numDescriptors(N),
    ShaderRegister(SR),
    RegisterSpace(RS),
    Flags(F)
{}

std::string RS_Layout::RootParameter::Range::GetCode() noexcept
{
    std::string code;
    return code = std::to_string(Type) + ',' + std::to_string(numDescriptors) + ',' + std::to_string(ShaderRegister) + ',' + std::to_string(RegisterSpace) + ',' + std::to_string(Flags);
}

RS_Layout::RootParameter::RootParameter(D3D12_SHADER_VISIBILITY V, Type t) noexcept
    :
    Visibility(V),
    type(t)
{}

RS_Layout::RootParameter& RS_Layout::RootParameter::AddRange(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT numDescriptors, UINT ShaderRegister, UINT RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAGS Flags) noexcept
{
    Ranges.emplace_back(T, numDescriptors, ShaderRegister, RegisterSpace, Flags);
    return *this;
}


RS_Layout::RootParameter::Range& RS_Layout::RootParameter::operator[](UINT index) noexcept
{
    return Ranges[index];
}

std::string RS_Layout::RootParameter::GetCode() noexcept
{
    std::string code;
    for (size_t i = 0; i < Ranges.size(); i++)
    {
        code += "{Range" + '[' + std::to_string(i) + "]:";
        code += Ranges[i].GetCode() + "}";
    }
    return code;
}

