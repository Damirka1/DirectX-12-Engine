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

	Engine_API FrameCommander(Window* pWindow, ResourceManager* pRM, bool SetToWindow = false) noexcept;
	Engine_API void SetBackgroundColor(float r, float g, float b) noexcept;
	Engine_API void ChangeBackgroundColor(float dr, float dg, float db) noexcept;
	Engine_API void Render() const;
	Engine_API ~FrameCommander() = default;

private:
	Graphics* pGraphics;
	ResourceManager* pRM;
	float bg[3] = { 0.0f, 0.0f, 0.0f };
};

#endif 
