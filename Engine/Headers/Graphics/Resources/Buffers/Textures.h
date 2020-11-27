#pragma once
#ifndef TEXTURES_HEADER
#define TEXTURES_HEADER
#include "..\Bindable.h"
#include "..\..\..\DirectXTex.h"

class Texture2D : public Bindable
{
public:
	Texture2D(std::unique_ptr<DirectX::ScratchImage> pImage, D3D12_RESOURCE_DESC* pDesc, bool OnlyPixelShader = false);
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle) override;
	~Texture2D() override;

private:
	ID3D12Resource* pBuffer = nullptr;
	ID3D12Resource* pCopyBuffer = nullptr;

	bool OnlyPixelShader;
	std::unique_ptr<DirectX::ScratchImage> pImage;
	D3D12_RESOURCE_DESC pDesc;
};


#endif
