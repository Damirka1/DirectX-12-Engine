#include "Application.h"


Application::Application(HINSTANCE hInstance)
	:
	Con(L"DirectX 12 Console")
{
	pWindow = new Window(hInstance, L"DirectX 12 Engine");
	gtx = new Graphics(pWindow->GetHWND());
	Con << gtx->GetInfo().c_str();
	pWindow->SetGraphics(gtx);
	pWindow->Show();
	RM = new ResourceManager();
	FC = new FrameCommander(gtx, RM);
	FC->SetBackgroundColor(0.5f, 0.5f, 0.5f);
	t = new Cube("Image\\bugatti-la-voiture-noire-roadster-rendering-lead-image.jpg", RM);
	k = new Keyboard();
	m = new Mouse();
	pWindow->AddHandler(k, "Keyboard");
	pWindow->AddHandler(m, "Mouse");
	RM->InitializeResources(gtx);
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		static float color[3] = { 0.4f, 0.4f, 0.4f };
		static float pos[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		
		auto tm = pWindow->TimerMark();

		if (k->KeyIsPressed("W"))
		{
			pos[2] += 1.0f * tm;
		}
		if (k->KeyIsPressed("S"))
		{
			pos[2] -= 1.0f * tm;
		}
		if (k->KeyIsPressed("A"))
		{
			pos[0] -= 1.0f * tm;
		}
		if (k->KeyIsPressed("D"))
		{
			pos[0] += 1.0f * tm;
		}
		if (k->KeyIsPressed("Z"))
		{
			pos[1] += 1.0f * tm;
		}
		if (k->KeyIsPressed("X"))
		{
			pos[1] -= 1.0f * tm;
		}
		if (k->KeyIsPressed("R"))
		{
			pos[3] += 1.0f * tm;
		}
		if (k->KeyIsPressed("F"))
		{
			pos[3] -= 1.0f * tm;
		}
		if (k->KeyIsPressed("E"))
		{
			pos[4] += 1.0f * tm;
		}
		if (k->KeyIsPressed("Q"))
		{
			pos[4] -= 1.0f * tm;
		}
		if (k->KeyIsPressed("C"))
		{
			pos[5] += 1.0f * tm;
		}
		if (k->KeyIsPressed("V"))
		{
			pos[5] -= 1.0f * tm;
		}

		

		
		t->Update(pos, 6);
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
	delete RM;
	delete t;
	delete k;
	delete m;
}
