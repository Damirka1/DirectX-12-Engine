#include "Application.h"
#include <random>

Application::Application(HINSTANCE hInstance)
	:
	Con(L"DirectX 12 Console")
{
	pWindow = new Window(hInstance, L"DirectX 12 Engine", 1280, 720);
	Con << pWindow->GetGraphics()->GetInfo().c_str();
	pWindow->Show();
	// Camera needs to initialize before resource manager initialization!
	cam = new Camera(pWindow);
	cam->SetSensitivity(0.005f);
	RM = new ResourceManager(pWindow);
	FC = new FrameCommander(pWindow, RM);
	FC->SetBackgroundColor(0.5f, 0.5f, 0.5f);
	k = pWindow->GetKeyboard();
	m = pWindow->GetMouse();
	
	r = new Rect(RM, "Rect", { 635.0f, 355.0f }, { 5, 5 });

	// Random numbers for cube's position.
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::minstd_rand gen(rd());
	std::uniform_real_distribution<float> disPosXY(-50.0f, 50.0f);
	std::uniform_real_distribution<float> disPosZ(4.0f, 100.0f);

	for (int i = 0; i < 64 * 64; i++)
	{
		Cubes.push_back(new Cube(RM, DirectX::XMFLOAT3{ disPosXY(gen), disPosXY(gen), disPosZ(gen) }));
	}

	RM->InitializeResources(pWindow);
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		static float color[3] = { 0.4f, 0.4f, 0.4f };

		auto tm = pWindow->TimerMark();
		{
			DirectX::XMFLOAT3 Translation = { 0.0f, 0.0f, 0.0f };

			if (k->KeyIsPressed(VK_SHIFT))
				cam->SetSpeed(4.0f);
			else
				cam->SetSpeed(1.0f);

			if (k->KeyIsPressed('W'))
				Translation.z += 10.0f * tm;
			if (k->KeyIsPressed('S'))
				Translation.z -= 10.0f * tm;
			if (k->KeyIsPressed('A'))
				Translation.x -= 10.0f * tm;
			if (k->KeyIsPressed('D'))
				Translation.x += 10.0f * tm;
			if (k->KeyIsPressed(VK_SPACE))
				Translation.y += 10.0f * tm;
			if (k->KeyIsPressed(VK_CONTROL))
				Translation.y -= 10.0f * tm;

			while (auto el = k->GetEvent())
			{
				auto& ev = el.value();

				if (ev == VK_ESCAPE)
				{
					if (!m->IsCursorEnabled())
					{
						pWindow->EnableCursor();
						m->DisableRawInput();
					}
					else
					{
						pWindow->DisableCursor();
						m->EnableRawInput();
					}
				}
			}
			
			if (!m->IsCursorEnabled())
			{
				while(const auto ev = m->GetRawData())
						cam->Rotate((float)ev->dx, (float)ev->dy);
			}

			cam->Translate(Translation);
		}
		
		for (Cube* obj : Cubes)
		{
			obj->Update();
		}

		pWindow->ProcessMessages();
		FC->SetBackgroundColor(color[0], color[1], color[2]);
		// Render.
		FC->Render();
	}
}

Application::~Application()
{
	delete pWindow;
	delete FC;
	delete RM;
	delete r;

	for (Cube* obj : Cubes)
	{
		delete obj;
	}
}
