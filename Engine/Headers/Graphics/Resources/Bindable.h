#pragma once
#ifndef BINDABLE_HEADER
#define BINDABLE_HEADER
#include "..\..\Graphics.h"

class Bindable
{
	friend class ResourceManager;
public:
	virtual void Bind(Graphics* pGraphics) = 0;
	virtual void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
	{}
	virtual void Initialize(Graphics* pGraphics)
	{}
	virtual ~Bindable()
	{}
	void SetKeys(std::string PSO, std::string RS)
	{
		this->PSO_key = PSO;
		this->RS_key = RS;
	}
	
protected:
	void SetHeapIndex(UINT Table, UINT Range, UINT Index)
	{
		this->Table = Table;
		this->Range = Range;
		this->Index = Index;
	}

	bool Initialized = false;
	std::string PSO_key = "", RS_key = "";
	UINT Table = 0u, Range = 0u, Index = 0u;
};

#endif