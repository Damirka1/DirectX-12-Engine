#pragma once
#ifndef ACTOR_HEADER
#define ACTOR_HEADER
#include <string>

class Actor
{
protected:
	std::string _Tag;
public:
	virtual void OnCreate() = 0;
	virtual void OnTick() = 0;

	void SetTag(std::string&& Tag)
	{
		_Tag = Tag;
	}

	std::string GetTag()
	{
		return _Tag;
	}
};

#endif
