#pragma once

#include "Resource.h"
#include "RootSignature.h"

class RTXStateObject : public Resource 
{
public:
	RTXStateObject(std::shared_ptr<RootSignature> pRootSignature);

	void Initialize(Graphics* pGraphics);

	void Bind(Graphics* pGraphics);

private:

	std::shared_ptr<RootSignature> pRootSignature;
	ID3D12StateObject* pStateObject;
	
	std::string KeyCode;
};