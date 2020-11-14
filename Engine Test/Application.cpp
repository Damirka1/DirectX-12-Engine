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
	t = new Triangle(gtx, RM);
	gtx->Initialize();
	k = new Keyboard();
	m = new Mouse();
	pWindow->AddHandler(k, "Keyboard");
	pWindow->AddHandler(m, "Mouse");
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		static float color[3] = { 0.6f, 0.6f, 0.6f };
		float pos[2] = { 0.0f, 0.0f };

		// Keyboard events.
		while (auto e = k->GetEvent())
		{
			auto& v = e.value();
			if (v == "F")
			{
				Con << L"Pay respect\n";
			}
			if (v == "R")
			{
				color[0] += 0.1f;
			}
			if (v == "G")
			{
				color[1] += 0.1f;
			}
			if (v == "B")
			{
				color[2] += 0.1f;
			}
			if (v == "Q")
			{
				color[0] = 0.0f;
				color[1] = 0.0f;
				color[2] = 0.0f;
			}
		}

		auto tm = pWindow->TimerMark();

		if (k->KeyIsPressed("W"))
		{
			pos[0] += 0.5f * tm;
		}
		if (k->KeyIsPressed("S"))
		{
			pos[0] -= 0.5f * tm;
		}
		if (k->KeyIsPressed("A"))
		{
			pos[1] -= 0.5f * tm;
		}
		if (k->KeyIsPressed("D"))
		{
			pos[1] += 0.5f * tm;
		}


		// Mouse events.
		while (auto ev = m->GetEvent())
		{
			auto& v = ev.value();
			if (v.WheelDown())
				Con << L"Wheel Down\n";
			if (v.WheelUp())
				Con << L"Wheel Up\n";
			if (v.WheelPressed())
				Con << L"Wheel Pressed\n";
			if (v.L_Pressed())
				Con << L"LeftButton Pressed\n";
			if (v.R_Pressed())
				Con << L"RightButton Pressed\n";
		}

		auto MousePos = m->GetPos();
		pWindow->SetWindowName(std::string("Mouse pos " + std::to_string(MousePos.first) + ":" + std::to_string(MousePos.second)).c_str());
		
		// Render.
		t->Update(color[0], color[1], color[2], pos[0], pos[1]);
		pWindow->ProcessMessages();
		gtx->Setup(0.3f, 0.3f, 0.3f);
		t->Draw(gtx);
		gtx->Execute();
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
