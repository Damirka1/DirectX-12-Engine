#include "Application.h"
#include <random>

Application::Application(HINSTANCE hInstance)
	:
	Con(L"DirectX 12 Console")
{
	pWindow = new Window(hInstance, L"DirectX 12 Engine", 1280, 720);
	gtx = new Graphics(pWindow->GetHWND(), 1280, 720);
	Con << gtx->GetInfo().c_str();
	pWindow->SetGraphics(gtx);
	pWindow->Show();
	RM = new ResourceManager(gtx);
	FC = new FrameCommander(gtx, RM);
	FC->SetBackgroundColor(0.5f, 0.5f, 0.5f);
	k = new Keyboard();
	m = new Mouse();
	pWindow->AddHandler(k, "Keyboard");
	pWindow->AddHandler(m, "Mouse");
	
	r = new Rect(RM, "Rect", { 627.5f, 347.5f }, { 25, 25 });

	// Random numbers for cube's position.
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::minstd_rand gen(rd());
	std::uniform_real_distribution<float> disPosXY(-50.0f, 50.0f);
	std::uniform_real_distribution<float> disPosZ(4.0f, 100.0f);

	for (int i = 0; i < 64; i++)
	{
		Cubes.push_back(new Cube(RM, DirectX::XMFLOAT3{ disPosXY(gen), disPosXY(gen), disPosZ(gen) }));
	}

	RM->InitializeResources(gtx);
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		static float color[3] = { 0.4f, 0.4f, 0.4f };

		
		auto tm = pWindow->TimerMark();

		
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
	delete gtx;
	delete FC;
	delete RM;
	delete k;
	delete m;
	delete r;

	for (Cube* obj : Cubes)
	{
		delete obj;
	}
}
