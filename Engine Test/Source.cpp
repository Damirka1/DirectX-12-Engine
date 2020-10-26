#include "..\Engine\Headers\Window.h"
#include "..\Engine\Headers\ResourceManager.h"
#include "..\Engine\Headers\Graphics.h"
#include "..\Engine\Models\Triangle.h"
#ifdef _DEBUG
#include "..\ConsoleDLL\Console.h"
#endif // DEBUG



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR pCmdLine, _In_ int)
{
#ifdef _DEBUG
	Console* Con = new Console(L"Test Console");
	*Con << L"Hello, World\n" << L"123";
#endif
	Window* pWindow = new Window(hInstance, L"DirectX 12 Engine");
	Graphics* gtx = new Graphics(pWindow->GetHWND());
	pWindow->SetGraphics(gtx);
	pWindow->Show();
	ResourceManager* RM = new ResourceManager();
	Triangle* t = new Triangle(gtx, RM);
	gtx->Initialize();

	while (pWindow->IsExist())
	{
		pWindow->ProcessMessages();
		gtx->Setup(0.5f, 0.5f, 0.5f);
		t->Draw(gtx);
		gtx->Execute();
	}
#ifdef _DEBUG
	delete Con;
#endif

	delete pWindow;
	delete gtx;
	delete RM;
	delete t;

	return 0;
}