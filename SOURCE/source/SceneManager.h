#pragma once
#include <vector>

#include "Singleton.h"
#include "Scene.h"

class SceneManager final : public Singleton<SceneManager>
{
public:
	SceneManager(const SceneManager& other) = delete;
	SceneManager(SceneManager&& other) = delete;
	SceneManager& operator=(const SceneManager& other) = delete;
	SceneManager& operator=(SceneManager&& other) = delete;
	~SceneManager() override = default;

	Scene& AddNewScene();

	Scene& GetScene();
	Scene& GotoScene(const int index);
	Scene& GotoNextScene();
	Scene& GotoPreviousScene();

private:
	friend class Singleton<SceneManager>;
	SceneManager();

	int m_CurrentIndex = 0;
	std::vector<Scene> m_Scenes{};
};