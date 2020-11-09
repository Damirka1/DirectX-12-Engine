#include "Application.h"


Application::Application(HINSTANCE hInstance)
{
#ifdef _DEBUG
	Con = new Console(L"Test Console");
	*Con << L"Hello, World\n" << L"123\n";
#endif
	pWindow = new Window(hInstance, L"DirectX 12 Engine");
	gtx = new Graphics(pWindow->GetHWND());
	pWindow->SetGraphics(gtx);
	pWindow->Show();
	RM = new ResourceManager();
	t = new Triangle(gtx, RM);
	gtx->Initialize();
	k = new Keyboard();
	pWindow->AddHandler(k, "Keyboard");
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		static float color[3] = { 0.6f, 0.6f, 0.6f };
		float pos[2] = { 0.0f, 0.0f };
		while (auto e = k->GetEvent())
		{
			auto& v = e.value();
			if (v == "F")
			{
				*Con << L"Pay respect\n";
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

		auto m = pWindow->TimerMark();

		if (k->KeyIsPressed("W"))
		{
			pos[0] += 0.5f * m;
		}
		if (k->KeyIsPressed("S"))
		{
			pos[0] -= 0.5f * m;
		}
		if (k->KeyIsPressed("A"))
		{
			pos[1] -= 0.5f * m;
		}
		if (k->KeyIsPressed("D"))
		{
			pos[1] += 0.5f * m;
		}


		

		t->Update(color[0], color[1], color[2], pos[0], pos[1]);
		pWindow->ProcessMessages();
		gtx->Setup(0.3f, 0.3f, 0.3f);
		t->Draw(gtx);
		gtx->Execute();
	}
}

Application::~Application()
{
#ifdef _DEBUG
	delete Con;
#endif

	delete pWindow;
	delete gtx;
	delete RM;
	delete t;
	delete k;
}
