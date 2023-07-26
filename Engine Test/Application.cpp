#include "Application.h"
#include <random>
#include <thread>

Application::Application(HINSTANCE hInstance)
{
	Core = new EngineCore(hInstance);
	Core->PrintGraphicsInfoToConsole();
	Core->ShowWindow();

	kb = Core->GetKeyboardInput();
	ms = Core->GetMouseInput();

	ms->EnableRawInput();

	pCamera = new Camera(std::make_pair<short, short>(1280, 720));
	pCamera->SetSensitivity(0.005f);

	Core->SetBackgroundColor(0.2f, 0.2f, 0.2f);

	pCamera->SetPos({ 0.0f, 20.0f, -30.0f });
	pCamera->SetRotation(DirectX::XMConvertToRadians(45), 0);

	pScene = Core->CreateScene();
	pScene->SetCamera(pCamera);

	/*models.push_back(Core->LoadModel("C:\\Home\\Graphics Projects\\3d models\\spider man\\Blender\\SpiderMan.obj", "Test1", 2.0f));
	models[0]->SetPos({ -5.0f, -3.0f, 4.0f });
	pScene->AddModel(models[0]);
	models.push_back(Core->LoadModel("C:\\Home\\Graphics Projects\\3d models\\spider man\\Blender\\SpiderMan.obj", "Test2", 3.0f));
	models[1]->SetPos({ 5.0f, -3.0f, 4.0f });
	pScene->AddModel(models[1]);

	/*models.push_back(Core->LoadModel("C:\\Home\\GraphicsProjects\\3d models\\M-B AMG ", "Test1", 1.0f));
	models.back()->SetPos({ 0.0f, 2.0f, 0.0f });
	pScene->AddModel(models.back())*/;

	/*models.push_back(Core->LoadModel("C:\\Home\\GraphicsProjects\\3d models\\123\\obj\\kindred.obj", "Test3"));
	models.back()->SetPos({0.0, 0.0f, 0.0f});
	pScene->AddModel(models.back());*/

	//models.push_back(Core->CreatePlane({0, -20, 0}, {0, 0, 0}));
	models.push_back(pScene->CreatePlane({ 0, -20, 0 }, { 0, 0, 0 }));

	//models.push_back(Core->CreatePlane({ 0, -20, -20 }, { DirectX::XMConvertToRadians(-90), 0, 0}));
	models.push_back(pScene->CreatePlane({ 0, -20, -20 }, { DirectX::XMConvertToRadians(-90), 0, 0 }));

	//models.push_back(Core->CreatePlane({ 0, -20, 20 }, { DirectX::XMConvertToRadians(90), 0, 0 }));
	models.push_back(pScene->CreatePlane({ 0, -20, 20 }, { DirectX::XMConvertToRadians(90), 0, 0 }));

	//models.push_back(Core->CreatePlane({ 20, -20, 0 }, { 0, 0, DirectX::XMConvertToRadians(-90) }));
	models.push_back(pScene->CreatePlane({ 20, -20, 0 }, { 0, 0, DirectX::XMConvertToRadians(-90) }));

	//models.push_back(Core->CreatePlane({ -20, -20, 0 }, { 0, 0, DirectX::XMConvertToRadians(90) }));
	models.push_back(pScene->CreatePlane({ -20, -20, 0 }, { 0, 0, DirectX::XMConvertToRadians(90) }));

	
	std::default_random_engine e;
	std::uniform_real_distribution<float> dis(-15, 16); // range [-15, 15]

	//const int threadCount = 16;

	//std::vector<std::shared_ptr<StaticMeshComponent>> thd[threadCount] = {};
	//std::thread th[threadCount];
	//
	//for (int i = 0; i < threadCount; i++)
	//{
	//	th[i] = std::thread([&](int index) {
	//		int v = index;
	//		for (int j = 0; j < 10; j++)
	//			thd[index].push_back(Core->CreateSphere({dis(e), dis(e), dis(e)}));
	//		}, i);
	//}

	//for (int i = 0; i < threadCount; i++)
	//{
	//	th[i].join();
	//}

	//for (int i = 0; i < threadCount; i++)
	//{

	//	for(auto& el : thd[i])
	//	{
	//		spheres.push_back(std::reinterpret_pointer_cast<Sphere>(el));
	//		pScene->AddModel(el);
	//	}

	//	/*std::copy(spheres.begin(), spheres.end(), std::back_inserter(thd[i]));
	//	pScene->AddModels(thd[i])*/;
	//}

	for (int i = 0; i < 50; i++)
	{
		//spheres.push_back(Core->CreateSphere({dis(e), dis(e), dis(e)}));
		spheres.push_back(pScene->CreateSphere({ dis(e), dis(e), dis(e) }));
	}

	//spheres.back()->SetPos({ 0.0f, 0.0f, 4.0f });
	Core->SetCurrentScene(pScene);
	Core->PrepareDX();
}

void Application::Run()
{
	static float z = 0.0f;
	static int index = 0;
	bool add = false;
	while (Core->WindowIsExist())
	{
		Core->SetupJobs();

		while (auto El = kb->GetEvent())
		{
			if (El == std::nullopt)
				continue;

			auto Ev = El.value();

			if (Ev == '1')
				Core->DisableCursor();
			else if (Ev == '2')
				Core->EnableCursor();
			else if (Ev == '3')
			{
				add = true;
			}

		}

		float speed = 1.0f;

		if (kb->KeyIsPressed(VK_SHIFT))
			speed = 10.0f;

		if (kb->KeyIsPressed('W'))
			pCamera->Translate({ 0.0f, 0.0f, 0.1f * speed });
		if (kb->KeyIsPressed('S'))
			pCamera->Translate({ 0.0f, 0.0f, -0.1f * speed });
		if (kb->KeyIsPressed('D'))
			pCamera->Translate({ 0.1f * speed, 0.0f, 0.0f });
		if (kb->KeyIsPressed('A'))
			pCamera->Translate({ -0.1f * speed , 0.0f, 0.0f });

		if (!ms->IsCursorEnabled())
		{
			while (auto El = ms->GetRawData())
			{
				if (El == std::nullopt)
					continue;
				auto delta = El.value();
				pCamera->Rotate(delta.dx, delta.dy);
			}
		}

		Core->Update();

		//for (auto m : spheres)
		//	m->UpdateBody(pCamera);

		//for (auto m : cubes)
		//	m->UpdateBody(pCamera);

		if (add)
		{
			//auto s = Core->CreateSphere({0, 5.0f, 0});
			//s->AddForce({2.2f, 0.0f, 20.0f});
			//spheres.push_back(s);
			spheres.push_back(pScene->CreateSphere({ 0, 5.0f, 0 }));

			//auto c = Core->CreateCube();
			////c->AddForce({ 2.2f, 0.0f, 20.0f });
			//cubes.push_back(c);
			//pScene->AddModel(c);


			add = false;
			z += 5.0f;
			index++;
		}

		Core->ExecuteJobs();
	}
}

Application::~Application()
{
	delete Core;
	delete pCamera;
}
