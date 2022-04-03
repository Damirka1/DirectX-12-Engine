#include "..\..\..\Headers\Graphics\Resources\Buffers\Textures.h"
#include "..\..\..\Headers\Graphics\Error_Check.h"

Texture2D::Texture2D(std::unique_ptr<DirectX::ScratchImage> pImage, D3D12_RESOURCE_DESC* pDesc, bool OnlyPixelShader) noexcept
	:
	pImage(std::move(pImage)),
	pDesc(*pDesc),
	OnlyPixelShader(OnlyPixelShader)
{
}

void Texture2D::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->SetGraphicsRootDescriptorTable(Index, pGpuHandle);
}

void Texture2D::Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
{
	ID3D12Device9* pDevice = pGraphics->GetDevice();

	if (!Initialized)
	{
		ID3D12GraphicsCommandList6* pCommandList = pGraphics->GetCommandList();

		Error_Check(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&pDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&pBuffer)));


		Error_Check(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(pBuffer, 0, 1)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pCopyBuffer)));


		UINT64 pixels = pImage->GetPixelsSize() / (pDesc.Width * pDesc.Height);

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = pImage->GetPixels();
		textureData.RowPitch = pDesc.Width * pixels;
		textureData.SlicePitch = textureData.RowPitch * pDesc.Height;

		UpdateSubresources(pCommandList, pBuffer, pCopyBuffer, 0, 0, 1, &textureData);
		D3D12_RESOURCE_STATES State = OnlyPixelShader ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pBuffer, D3D12_RESOURCE_STATE_COPY_DEST, State));

		pGraphics->AddToRelease(pCopyBuffer);

		Initialized = true;
	}

	// Describe and create a SRV for the texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = pDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	pDevice->CreateShaderResourceView(pBuffer, &srvDesc, pHandle);
}

Texture2D::~Texture2D()
{
	pBuffer->Release();
	pBuffer = nullptr;
}
