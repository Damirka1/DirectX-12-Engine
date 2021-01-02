#include "Application.h"
#include <random>

class Listener : public EventListener
{
	void ListenKeyboardEvents(Drawable* pObject, KeyEvent* pEvent, Window* pWindow) override
	{
		if (*pEvent == 'D')
		{
			pObject->Translate({ 0.2f, 0.0f, 0.0f });
			pObject->Update();
			pWindow->UpdateWindow();
		}
		else if (*pEvent == 'A')
		{
			pObject->Translate({ -0.2f, 0.0f, 0.0f });
			pObject->Update();
			pWindow->UpdateWindow();
		}
		else if (*pEvent == 'W')
		{
			pObject->Translate({ 0.0f, 0.2f, 0.0f });
			pObject->Update();
			pWindow->UpdateWindow();
		}
		else if (*pEvent == 'S')
		{
			pObject->Translate({ 0.0f, -0.2f, 0.0f });
			pObject->Update();
			pWindow->UpdateWindow();
		}
	}
};

static Listener* l = new Listener();

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
	c->AddEventListener(l);
	
	r = new Rect(RM, "Rect", { 100, 100 }, { 100, 100 });
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
	r->GetDefaultListener()->OnMouseMove = [](UI_Element* This, Window* pWindow)
	{
		auto* Mouse = pWindow->GetMouse();
		if (Mouse->IsLbPressed())
		{
			auto ElPos = This->GetPos();
			auto MPos = Mouse->GetPos();

			This->SetPos({ (float)MPos.first, (float)MPos.second, 0.0f });
			This->Update();
			pWindow->UpdateWindow();
		}
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

		/*while (auto el = pWindow->GetKeyboard()->GetEvent())
		{
			auto ev = el.value();
			for (auto& l : *c->GetEventListeners())
			{
				l->ListenKeyboardEvents(c, &ev, pWindow);
			}
		}*/

		//c->Update();
		pWindow->ProcessMessages();
		//FC->SetBackgroundColor(color[0], color[1], color[2]);
		// Render.
		//FC->Render();
	}
}

Application::~Application()
{
	delete pWindow;
	delete FC;
	delete RM;
	delete r;
	delete c;
	delete l;
}
