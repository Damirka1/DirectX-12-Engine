#include "Pass.h"

#include "../../Graphics.h"
#include "../../ResourceManager.h"

Pass::Pass(Graphics* pGraphics, ResourceManager* pRM)
	:
	pGraphics(pGraphics),
	pRM(pRM)
{
}
