#pragma once
#ifndef CHARACTER_HEADER
#define CHARACTER_HEADER
#include "Pawn.h"
#include "../Model.h"

class Character : public Pawn
{
	std::shared_ptr<Model> _Model;

public:

	Character(std::shared_ptr<Camera> Camera, std::shared_ptr<Model> Model)
		:
		Pawn(Camera),
		_Model(Model)
	{
	}

	virtual void OnCreate() override;
	virtual void OnTick() override;

};


#endif