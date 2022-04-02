#pragma once
#ifndef PAWN_HEADER
#define PAWN_HEADER
#include <memory>

#include "../Input/Camera.h"
#include "Actor.h"

class Pawn : public Actor
{
protected:
	std::shared_ptr<Camera> _Camera;

public:
	Pawn() = delete;
	Pawn(std::shared_ptr<Camera> Cam)
		:
		_Camera(Cam)
	{

	}

	virtual void OnCreate() override;
	virtual void OnTick() override;

};

#endif