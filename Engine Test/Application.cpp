#include "Application.h"
#include <random>

Application::Application(HINSTANCE hInstance)
	:
	Con(L"DirectX 12 Console")
{
	pWindow = new Window(hInstance, L"DirectX 12 Engine", 1280, 720);
	Con << pWindow->GetGraphics()->GetInfo().c_str();
	pWindow->Show();
	cam = new Camera(pWindow->GetGraphicsResolution());
	cam->SetSensitivity(0.005f);
	RM = new ResourceManager(pWindow);
	FC = new FrameCommanderHWND(pWindow, RM);
	FC->SetBackgroundColor(0.5f, 0.5f, 0.5f);
	pWindow->AddHandler(FC, "FC");
	k = pWindow->GetKeyboard();
	m = pWindow->GetMouse();
	c = new Cube(RM, { 0.0f, 5.0f, 20.0f });
	
	r = new Rect(RM, "Rect", { 300, 300 }, { 50, 100 });
	r->GetDefaultListener()->OnMouseEnter = [](UI_Element* This, Window* pWindow)
	{
		static_cast<Rect*>(This)->SetColor({ 1, 1, 1 });
		pWindow->UpdateWindow();
	};
	r->GetDefaultListener()->OnMouseLeave = [](UI_Element* This, Window* pWindow)
	{
		static_cast<Rect*>(This)->SetColor({ 1.0f, 0.5f, 0.5f });
		pWindow->UpdateWindow();
	};
	r->GetDefaultListener()->OnMouseClick = [](UI_Element* This, Window* pWindow)
	{
		MessageBoxW(nullptr, L"Hello world", L"Test", MB_OK);
	};

	pWindow->AddElement(r);
	
	RM->InitializeResources(pWindow);

	RM->SetCamera(cam);
	
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		static float color[3] = { 0.4f, 0.4f, 0.4f };

		auto tm = pWindow->TimerMark();

		auto mPos = pWindow->GetMouse()->GetPos();
		float x = mPos.first, y = mPos.second;
		pWindow->SetWindowName(std::string("x: " + std::to_string(x) + " y:" + std::to_string(y)).c_str());

		c->Update();
		pWindow->ProcessMessages();
		//FC->SetBackgroundColor(color[0], color[1], color[2]);
		// Render.
		//FC->Render();
		Sleep(1);
	}
}

Application::~Application()
{
	delete pWindow;
	delete FC;
	delete RM;
	delete r;
	delete c;
}
