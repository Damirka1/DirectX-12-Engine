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


	auto Render = [&](std::unordered_map<std::string, ResourceManager::PipeLineResources>& Resources)
	{
		// First bind pipelinestate object and heap.
		for (auto& PSO : Resources)
		{
			PSO.second.pPipeLineStateObject->Bind(pGraphics);

			// Next bind rootsignatures.
			for (auto& RS : PSO.second.RootSignatures)
			{
				RS.second.pRootSignature->Bind(pGraphics);
				pRM->Heap.Bind(pGraphics);

				// And finaly render objects.
				for (auto& obj : RS.second.DrawIndexed)
				{
					obj.second.DrawIndexed(pGraphics);
				}
			}
		}
	};

	Render(pRM->Resources);

	Render(pRM->UI_Resources);

	pGraphics->Execute();
}
