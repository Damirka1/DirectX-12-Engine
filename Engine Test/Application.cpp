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
	pCamera = new Camera(pWindow->GetGraphicsResolution());
	pCamera->SetSensitivity(0.005f);
	pResourceManager = new ResourceManager(pWindow);
	pFrameCommander = new FrameCommander(pWindow, pResourceManager);
	pFrameCommander->SetBackgroundColor(0.2f, 0.2f, 0.2f);

	pScene = new Scene();
	pScene->SetCamera(pCamera);

	models.push_back(new Model(pResourceManager, "C:\\Home\\Blender Models\\Characters\\Tiefling Rogue\\Obj\\Triefling Rogue.obj", 0.025f));
	models[0]->SetPos({ -5.0f, -3.0f, 4.0f });
	models.push_back(new Model(pResourceManager, "C:\\Home\\Blender Models\\Nissan S30\\Nissan S30.obj"));
	pScene->AddModel(models[0]);
	pScene->AddModel(models[1]);

	Con << std::to_wstring(pWindow->TimerPeek()).c_str();
	pFrameCommander->SetScene(pScene);
	pFrameCommander->PrepareAllResources();
	Con << std::to_wstring(pWindow->TimerPeek()).c_str();
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
			pCamera->Translate({ 0.0f, 0.0f, 0.1f * speed });
		if (kb->KeyIsPressed('S'))
			pCamera->Translate({ 0.0f, 0.0f, -0.1f * speed });
		if (kb->KeyIsPressed('D'))
			pCamera->Translate({ 0.1f * speed, 0.0f, 0.0f });
		if (kb->KeyIsPressed('A'))
			pCamera->Translate({ -0.1f * speed , 0.0f, 0.0f });


		auto ms = pWindow->GetMouse();
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

		for (Model* m : models)
			m->Update();

		// Render.
		pFrameCommander->Render();
	}
}

Application::~Application()
{
	delete pWindow;
	delete pFrameCommander;
	delete pResourceManager;
	delete pCamera;
	delete pScene;
	for (Model* m : models)
		delete m;
}
