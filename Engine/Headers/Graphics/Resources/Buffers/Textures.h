#pragma once
#ifndef TEXTURES_HEADER
#define TEXTURES_HEADER
#include "..\Resource.h"
#include "..\..\..\DirectXTex.h"

class Texture2D : public Resource
{
public:
	Texture2D(std::unique_ptr<DirectX::ScratchImage> pImage, D3D12_RESOURCE_DESC* pDesc, bool OnlyPixelShader = false) noexcept;
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics) override;
	~Texture2D() override;

private:
	ID3D12Resource* pBuffer = nullptr;
	ID3D12Resource* pCopyBuffer = nullptr;

	bool OnlyPixelShader;
	std::unique_ptr<DirectX::ScratchImage> pImage;
	D3D12_RESOURCE_DESC pDesc;
};


#endif
