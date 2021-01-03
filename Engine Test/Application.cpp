#include "Application.h"
#include <random>

class Listener : public EventListener
{
	// Random numbers for cube's position.
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::minstd_rand gen;
	std::uniform_real_distribution<float> dforx;
	std::uniform_real_distribution<float> dfory;
	std::uniform_real_distribution<float> Spawn;
	float dx, dy;

public:
	Listener()
	{
		gen = std::minstd_rand(rd());
		dforx = std::uniform_real_distribution<float>(-0.05f, 0.05f);
		dfory = std::uniform_real_distribution<float>(0.1f, 0.2f);
		Spawn = std::uniform_real_distribution<float>(-2.0f, 2.0f);
		dx = dforx(gen);
		dy = dfory(gen);
	}

	void Script(Drawable* pObject, Window* pWindow) override
	{
		
		if ((int)pObject->GetPos().y != 8)
		{
			pObject->Translate({ dx, dy, 0.0f });
			pObject->Update();
			//pWindow->UpdateWindow();
		}
		else
		{
			auto Pos = pObject->GetPos();
			Pos.y = -4 + Spawn(gen);
			Pos.x = 2 + Spawn(gen);
			pObject->SetPos(Pos);
			dx = dforx(gen);
			dy = dfory(gen);
		}

	}
};


std::pair<float, float> Delta;
bool CanMove = false;

void Click(UI_Element* This, Window* pWindow)
{
	auto* Mouse = pWindow->GetMouse();
	auto ElPos = This->GetPos();
	auto MPos = Mouse->GetPos();

	Delta.first = ElPos.x - MPos.first;
	Delta.second = ElPos.y - MPos.second;
	CanMove = true;
}

class MoveRect : public EventListener
{
	
	void Script(Drawable* pObject, Window* pWindow) override
	{
		auto* Mouse = pWindow->GetMouse();
		if (Mouse->IsLbPressed())
		{
			if (CanMove)
			{
				auto ElPos = pObject->GetPos();
				auto MPos = Mouse->GetPos();

				pObject->SetPos({ (float)MPos.first + Delta.first, (float)MPos.second + Delta.second, 0.0f });
				pObject->Update();
				//pWindow->UpdateWindow();
			}
		}
		else
			CanMove = false;
	}

};



static const int Count = 30;
static Listener* l[Count];
static MoveRect* mr = new MoveRect();
static Cube* c[Count];

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
	//pWindow->AddHandler(FC, "FC");

	for (int i = 0; i < Count; i++)
	{
		c[i] = new Cube(RM, { 2.0f, -4.0f, 20.0f });
		l[i] = new Listener();
		c[i]->AddEventListener(l[i]);
	}
	
	r = new Rect(RM, "Rect", { 100, 100 }, { 50, 50 });
	r->GetDefaultListener()->OnMouseEnter = [](UI_Element* This, Window* pWindow)
	{
		static_cast<Rect*>(This)->SetColor({ 1, 1, 1 });
		//pWindow->UpdateWindow();
	};
	r->GetDefaultListener()->OnMouseLeave = [](UI_Element* This, Window* pWindow)
	{
		static_cast<Rect*>(This)->SetColor({ 1.0f, 0.5f, 0.5f });
		//pWindow->UpdateWindow();
	};
	r->GetDefaultListener()->OnMouseLbClick = Click;
	r->AddEventListener(mr);

	

	pWindow->AddElement(r);
	
	RM->InitializeResources(pWindow);

	RM->SetCamera(cam);
	
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		auto mPos = pWindow->GetMouse()->GetPos();
		float x = mPos.first, y = mPos.second;
		pWindow->SetWindowName(std::string("x: " + std::to_string(x) + " y:" + std::to_string(y)).c_str());

		pWindow->ProcessMessages();

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
	delete cam;
	delete mr;
	for (int i = 0; i < Count; i++)
	{
		delete c[i];
		delete l[i];
	}
}
