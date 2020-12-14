#include "..\Headers\FrameCommander.h"
#include "..\Headers\Window.h"

FrameCommander::FrameCommander(Window* pWindow, ResourceManager* pRM, bool SetToWindow) noexcept
	:
	pGraphics(pWindow->GetGraphics()),
	pRM(pRM)
{
	if (SetToWindow)
	{
		// TO DO: set to window.
	}
}

void FrameCommander::SetBackgroundColor(float r, float g, float b) noexcept
{
	bg[0] = r;
	bg[1] = g;
	bg[2] = b;
}

void FrameCommander::ChangeBackgroundColor(float dr, float dg, float db) noexcept
{
	bg[0] += dr;
	bg[1] += dg;
	bg[2] += db;
}

void FrameCommander::Render() const
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
