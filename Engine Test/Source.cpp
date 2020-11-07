#include "Application.h"
#include "../Engine/Headers/Graphics/GraphicsException.h"


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR pCmdLine, _In_ int)
{
	Application* app = nullptr;

	try
	{
		app = new Application(hInstance);
		app->Run();
	}
	catch (GraphicsException& ex)
	{
		MessageBoxA(nullptr, ex.What().c_str(), ex.GetType(), MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Unknown error", MB_OK | MB_ICONERROR);
	}

	delete app;

	return 0;
}