#include "Pass.h"

#include "../../Graphics.h"
#include "../../ResourceManager.h"

Pass::Pass(Graphics* pGraphics)
	:
	pGraphics(pGraphics)
{
	pRM = new ResourceManager(pGraphics);
}

ResourceManager* Pass::GetResourceManager()
{
	return pRM;
}

Pass::~Pass()
{
	delete pRM;
}

void Pass::Bind()
{
	pRM->BindHeap();
}
