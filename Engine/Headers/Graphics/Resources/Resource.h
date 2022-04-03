#pragma once
#ifndef RESOURCE_HEADER
#define RESOURCE_HEADER
#include "../../Graphics.h"
#include "Heap/Descriptor.h"

class Resource
{
	friend class ResourceManager;
public:
	virtual void Bind(Graphics* pGraphics)
	{

	};

	void SetDescriptor(Descriptor* pDescriptor)
	{
		this->pDescriptor = pDescriptor;
	}

	void RemoveDescriptor()
	{
		pDescriptor = nullptr;
	}

	bool HasDescriptor()
	{
		return pDescriptor;
	}
protected:
	virtual void Initialize(Graphics* pGraphics)
	{
	};

	virtual ~Resource()
	{
		if (pDescriptor)
			pDescriptor->RemoveResource();
	}

	void SetHeapIndex(UINT Index) noexcept
	{
		this->Index = Index;
	}

	std::string Name;
	bool Initialized = false;
	UINT Index = 0u;

	Descriptor* pDescriptor = nullptr;

};

#endif