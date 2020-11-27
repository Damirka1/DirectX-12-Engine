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
	t = new Triangle("Image\\bugatti-la-voiture-noire-roadster-rendering-lead-image.jpg", gtx, RM);
	t2 = new Triangle("Image\\kung-fu-panda-9.jpg", gtx, RM);
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
		static float color[3] = { 0.6f, 0.6f, 0.6f };
		float pos[2] = { 0.0f, 0.0f };
		float pos2[2] = { 0.0f, 0.0f };

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

		if (k->KeyIsPressed("I"))
		{
			pos2[0] += 0.5f * tm;
		}
		if (k->KeyIsPressed("K"))
		{
			pos2[0] -= 0.5f * tm;
		}
		if (k->KeyIsPressed("J"))
		{
			pos2[1] -= 0.5f * tm;
		}
		if (k->KeyIsPressed("L"))
		{
			pos2[1] += 0.5f * tm;
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
		t2->Update(color[0], color[1], color[2], pos2[0], pos2[1]);
		pWindow->ProcessMessages();
		
		FC->Render();
	}
}

Application::~Application()
{
	delete pWindow;
	delete gtx;
	delete RM;
	delete t;
	delete t2;
	delete k;
	delete m;
}
