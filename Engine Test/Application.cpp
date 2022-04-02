#include "Application.h"
#include <random>

Application::Application(HINSTANCE hInstance)
{
	Core = new EngineCore(hInstance);
	Core->PrintGraphicsInfoToConsole();
	Core->ShowWindow();

	kb = Core->GetKeyboardInput();
	ms = Core->GetMouseInput();

	ms->EnableRawInput();

	pCamera = new Camera(std::make_pair<short, short>(1280, 720));
	pCamera->SetSensitivity(0.005f);

	Core->SetBackgroundColor(0.2f, 0.2f, 0.2f);

	pScene = Core->CreateScene();
	pScene->SetCamera(pCamera);

	models.push_back(Core->LoadModel("C:\\Home\\Graphics Projects\\3d models\\spider man\\Blender\\SpiderMan.obj", "Test1", 2.0f));
	models[0]->SetPos({ -5.0f, -3.0f, 4.0f });
	pScene->AddModel(models[0]);
	models.push_back(Core->LoadModel("C:\\Home\\Graphics Projects\\3d models\\spider man\\Blender\\SpiderMan.obj", "Test2", 3.0f));
	models[1]->SetPos({ 5.0f, -3.0f, 4.0f });
	pScene->AddModel(models[1]);

	models.push_back(Core->LoadModel("C:\\Home\\Graphics Projects\\3d models\\123\\obj\\kindred.obj", "Test3"));
	pScene->AddModel(models[2]);
	Core->SetCurrentScene(pScene);
	Core->PrepareDX();
}

void Application::Run()
{
	bool add = false;
	while (Core->WindowIsExist())
	{
		Core->SetupJobs();

		if (add)
		{
			models.push_back(Core->LoadModel("C:\\Home\\Graphics Projects\\3d models\\spider man\\Blender\\SpiderMan.obj", "Test1", 1.0f));
			models[3]->SetPos({ -5.0f, -3.0f, -4.0f });
			pScene->AddModel(models[3]);
			add = false;
		}

		

		while (auto El = kb->GetEvent())
		{
			if (El == std::nullopt)
				continue;

			auto Ev = El.value();

			if (Ev == '1')
				Core->DisableCursor();
			else if (Ev == '2')
				Core->EnableCursor();
			else if (Ev == '3')
			{
				add = true;
			}

		}

		float speed = 1.0f;

		if (kb->KeyIsPressed(VK_SHIFT))
			speed = 10.0f;

		if (kb->KeyIsPressed('W'))
			pCamera->Translate({ 0.0f, 0.0f, 0.1f * speed });
		if (kb->KeyIsPressed('S'))
			pCamera->Translate({ 0.0f, 0.0f, -0.1f * speed });
		if (kb->KeyIsPressed('D'))
			pCamera->Translate({ 0.1f * speed, 0.0f, 0.0f });
		if (kb->KeyIsPressed('A'))
			pCamera->Translate({ -0.1f * speed , 0.0f, 0.0f });

		if (!ms->IsCursorEnabled())
		{
			while (auto El = ms->GetRawData())
			{
				if (El == std::nullopt)
					continue;
				auto delta = El.value();
				pCamera->Rotate(delta.dx, delta.dy);
			}
		}

		for (auto m : models)
			m->Update();

		Core->ExecuteJobs();

	}
}

Application::~Application()
{
	delete Core;
	delete pCamera;
}
