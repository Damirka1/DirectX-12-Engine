#include "..\Headers\FrameCommander.h"

FrameCommander::FrameCommander(Graphics* pGraphics, ResourceManager* pRM)
	:
	pGraphics(pGraphics),
	pRM(pRM)
{

}

void FrameCommander::SetBackgroundColor(float r, float g, float b)
{
	bg[0] = r;
	bg[1] = g;
	bg[2] = b;
}

void FrameCommander::ChangeBackgroundColor(float dr, float dg, float db)
{
	bg[0] += dr;
	bg[1] += dg;
	bg[2] += db;
}

void FrameCommander::Render()
{
	pGraphics->Setup(bg[0], bg[1], bg[2]);

	// First bind pipelinestate object and heap.
	for (auto PSO_begin = pRM->Resources.begin(), PSO_end = pRM->Resources.end(); PSO_begin != PSO_end; ++PSO_begin)
	{
		PSO_begin->second.pPipeLineStateObject->Bind(pGraphics);

		// Next bind rootsignatures.
		for (auto RS_begin = PSO_begin->second.RootSignatures.begin(), RS_end = PSO_begin->second.RootSignatures.end(); RS_begin != RS_end; ++RS_begin)
		{
			RS_begin->second.pRootSignature->Bind(pGraphics);
			pRM->Heap.Bind(pGraphics);

			// And finaly render objects.
			for (UINT i = 0; i < RS_begin->second.Count; i++)
			{
				RS_begin->second.pDrawables[i]->Bind(pGraphics);
				RS_begin->second.pDrawables[i]->DrawIndexed(pGraphics);
			}
		}
	}

	pGraphics->Execute();
}
