#pragma once
#ifndef DRAWABLE_HEADER
#define DRAWABLE_HEADER
#include <vector>
#include <memory>
#include "Bindable.h"
#include "..\..\ResourceManager.h"
#include "BindablesHeader.h"


class Drawable
{
	friend class ResourceManager;
	friend class FrameCommander;
	friend class DrawableArray;
protected:
	void AddBindable(std::shared_ptr<Bindable> Bindable);

	void Bind(Graphics* pGraphics);

	void SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB);

protected:
	bool UI_element = false;

private:
	std::vector<std::shared_ptr<Bindable>> Bindables;

	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::vector<std::shared_ptr<Bindable>> InitializeHeap_list;
	std::vector<std::shared_ptr<Bindable>> InitializeCommon_list;
	// Index of Descriptor heap.
	char DescHeapIndex = -1;
	
	std::string PSO_Key, RS_Key;
};

#endif