#include "pch.h"
//#undef main

#ifdef _DEBUG
	#include <vld.h>
#endif


//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "EDirectxRenderer.h"
#include "Structs.h"
#include <vector>
#include "SceneManager.h"
#include "Texture.h"
#include "EOBJParser.h"

#include "VehicleMaterial.h"
#include "ExhaustMaterial.h"
#include "SoftwareRenderer.h"
#include "TriangleMesh.h"

enum class FilterMode
{
	Point = 0,
	Linear = 1,
	Anisotropic = 2,
	EndDoNotUse = 3
};

using namespace Elite;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Rasterizer - Adam Benamar-Belkacem, 2DAE06",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto directxRenderer{ std::make_unique<Elite::DirectxRenderer>(pWindow) };
	auto softwareRenderer{ std::make_unique<Elite::SoftwareRenderer>(pWindow, 
		new Texture("Resources/vehicle_diffuse.png", directxRenderer->GetDevice()),
		new Texture("Resources/vehicle_normal.png", directxRenderer->GetDevice())) };

	SceneManager& sceneManager{ SceneManager::GetInstance() };
	
	Scene& scene = sceneManager.GetScene();
	scene.SetCamera(new Camera(width, height));

	TriangleMesh* pTriangleMeshVehicle{ nullptr };
	Mesh* pMeshVehicle{ nullptr };
	Mesh* pFireFX{ nullptr };
	{
		{
			std::vector<IVertex> vertices{};
			std::vector<uint32_t> indices{};
			ParseOBJ("Resources/vehicle.obj", vertices, indices);

			VehicleMaterial* pVehicleMaterial{ new VehicleMaterial(directxRenderer->GetDevice(), L"Resources/PosCol3D.fx") };
			pVehicleMaterial->SetDiffuseTexture(new Texture("Resources/vehicle_diffuse.png", directxRenderer->GetDevice()));
			pVehicleMaterial->SetNormalMap(new Texture("Resources/vehicle_normal.png", directxRenderer->GetDevice()));
			pVehicleMaterial->SetSpecularMap(new Texture("Resources/vehicle_specular.png", directxRenderer->GetDevice()));
			pVehicleMaterial->SetGlossinessMap(new Texture("Resources/vehicle_gloss.png", directxRenderer->GetDevice()));

			pMeshVehicle = new Mesh(directxRenderer->GetDevice(), vertices, indices, pVehicleMaterial);
			pMeshVehicle->SetPosition({ 0,0,50.f });
			scene.AddMesh(pMeshVehicle);

			pTriangleMeshVehicle = new TriangleMesh(FPoint3{ 0,0,50.f }, vertices, indices);
			scene.AddGeometry(pTriangleMeshVehicle);
		}

		{
			std::vector<IVertex> vertices{};
			std::vector<uint32_t> indices{};
			ParseOBJ("Resources/fireFX.obj", vertices, indices);

			ExhaustMaterial* material
			{
				new ExhaustMaterial (directxRenderer->GetDevice(), L"Resources/Exhaust.fx", 
					new Texture("Resources/fireFX_diffuse.png", directxRenderer->GetDevice()))
			};

			pFireFX = new Mesh(directxRenderer->GetDevice(), vertices, indices, material);
			pFireFX->SetPosition({ 0,0,50.f });
			scene.AddMesh(pFireFX);
		}
	}
	
	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool hardwarerasterizer = true;
	bool rotateVehicle = false;
	FilterMode filterMode = FilterMode::Point;

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_f && hardwarerasterizer)
				{
					for (auto* pMesh : scene.GetMeshes())
					{
						pMesh->GetMaterial()->NextTechnique();
					}

					int newFilterMode = (static_cast<int>(filterMode) + 1) % static_cast<int>(FilterMode::EndDoNotUse);
					filterMode = static_cast<FilterMode>(newFilterMode);

					switch (filterMode)
					{
					case FilterMode::Point:
						std::cout << "Filtering set to Point mode\n";
						break;
					case FilterMode::Linear:
						std::cout << "Filtering set to Linear mode\n";
						break;
					case FilterMode::Anisotropic:
						std::cout << "Filtering set to Anisotropic mode\n";
						break;
					}
				}

				if (e.key.keysym.sym == SDLK_e)
				{
					hardwarerasterizer = !hardwarerasterizer;
					if (hardwarerasterizer)
						std::cout << "Switched to Hardware Rasterizer (DirectX)\n";
					else
						std::cout << "Switched to Software Rasterizer\n";
				}

				if (e.key.keysym.sym == SDLK_t)
				{
					pFireFX->SetIsActive(!pFireFX->GetIsActive());
					if (!pFireFX->GetIsActive())
						std::cout << "FireFX mesh hidden\n";
					else
						std::cout << "FireFX mesh visible\n";
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_R)
					rotateVehicle = !rotateVehicle;
				break;
			}
		}

		// Update
		scene.GetCamera()->Update(pTimer->GetElapsed());
		if (rotateVehicle)
		{
			Elite::FMatrix3 rotation{ MakeRotationY(ToRadians(45.f * pTimer->GetElapsed())) };
			pTriangleMeshVehicle->SetForward(rotation * pTriangleMeshVehicle->GetForward());
			// todo: rotate Mesh
			pMeshVehicle->SetForward(rotation * pMeshVehicle->GetForward());
		}

		//--------- Render ---------
		if (hardwarerasterizer)
		{
			directxRenderer->Render();
		}
		else
		{
			softwareRenderer->Render();
		}

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

	}
	pTimer->Stop();

	//Shutdown "framework"
	ShutDown(pWindow);
	return 0;
}