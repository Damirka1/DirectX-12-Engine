#include "Application.h"


Application::Application(HINSTANCE hInstance)
{
#ifdef _DEBUG
	Con = new Console(L"Test Console");
	*Con << L"Hello, World\n" << L"123";
#endif
	pWindow = new Window(hInstance, L"DirectX 12 Engine");
	gtx = new Graphics(pWindow->GetHWND());
	pWindow->SetGraphics(gtx);
	pWindow->Show();
	RM = new ResourceManager();
	t = new Triangle(gtx, RM);
	gtx->Initialize();
}

void Application::Run()
{
	while (pWindow->IsExist())
	{
		float time = pWindow->TimerPeek();
		t->Update(static_cast<float>(std::sin(time)), static_cast<float>(std::cos(time)), 0.5f);
		pWindow->ProcessMessages();
		gtx->Setup(0.5f, 0.5f, 0.5f);
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

}
