#pragma once
#ifndef FRAME_COMMANDER_HEADER
#define FRAME_COMMANDER_HEADER
#include "Graphics.h"
#include "ResourceManager.h"

class FrameCommander
{
public:
	FrameCommander() = delete;
	FrameCommander(FrameCommander&) = delete;

	Engine_API FrameCommander(Graphics* pGraphics, ResourceManager* pRM);
	Engine_API void SetBackgroundColor(float r, float g, float b);
	Engine_API void ChangeBackgroundColor(float dr, float dg, float db);
	Engine_API void Render();

private:
	Graphics* pGraphics;
	ResourceManager* pRM;
	float bg[3] = { 0.0f, 0.0f, 0.0f };
};

#endif 
