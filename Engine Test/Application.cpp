#include "Application.h"
#include <random>

Application::Application(HINSTANCE hInstance)
	:
	Con(L"DirectX 12 Console")
{
	pWindow = new Window(hInstance, L"DirectX 12 Engine", 1280, 720);
	Con << pWindow->GetGraphics()->GetInfo().c_str();
	pWindow->Show();
	pWindow->GetMouse()->EnableRawInput();
	cam = new Camera(pWindow->GetGraphicsResolution());
	cam->SetSensitivity(0.005f);
	RM = new ResourceManager(pWindow);
	FC = new FrameCommanderHWND(pWindow, RM);
	FC->SetBackgroundColor(0.2f, 0.2f, 0.2f);

	// Random numbers for cube's position.
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::minstd_rand gen(rd());
	std::uniform_real_distribution<float> disPosXY(-50.0f, 50.0f);
	std::uniform_real_distribution<float> disPosZ(4.0f, 100.0f);

	for (int i = 0; i < 64 * 64; i++)
	{
		Cubes.push_back(new Cube(RM, DirectX::XMFLOAT3{ disPosXY(gen), disPosXY(gen), disPosZ(gen) }));
	}

	Con << std::to_wstring(pWindow->TimerPeek()).c_str();

	RM->InitializeResources(pWindow);

	RM->SetCamera(cam);
	
}

void Application::Run()
{
	while (pWindow->IsExist())
	{

		pWindow->ProcessMessages();

		auto kb = pWindow->GetKeyboard();

		while (auto El = kb->GetEvent())
		{
			if (El == std::nullopt)
				continue;

			auto Ev = El.value();

			if (Ev == '1')
				pWindow->DisableCursor();
			else if (Ev == '2')
				pWindow->EnableCursor();
		}

		float speed = 1.0f;

		if (kb->KeyIsPressed(VK_SHIFT))
			speed = 10.0f;

		if (kb->KeyIsPressed('W'))
			cam->Translate({ 0.0f, 0.0f, 0.1f * speed });
		if (kb->KeyIsPressed('S'))
			cam->Translate({ 0.0f, 0.0f, -0.1f * speed });
		if (kb->KeyIsPressed('D'))
			cam->Translate({ 0.1f * speed, 0.0f, 0.0f });
		if (kb->KeyIsPressed('A'))
			cam->Translate({ -0.1f * speed , 0.0f, 0.0f });


		auto ms = pWindow->GetMouse();
		if (!ms->IsCursorEnabled())
		{
			while (auto El = ms->GetRawData())
			{
				if (El == std::nullopt)
					continue;
				auto delta = El.value();
				cam->Rotate(delta.dx, delta.dy);
			}
		}

		for (Cube* obj : Cubes)
		{
			obj->Update();
		}

		// Render.
		FC->Render();
	}
}

Application::~Application()
{
	delete pWindow;
	delete FC;
	delete RM;
	delete cam;

	for (Cube* obj : Cubes)
	{
		delete obj;
	}
}
