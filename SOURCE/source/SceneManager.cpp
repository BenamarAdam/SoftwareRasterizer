#include "pch.h"
#include "SceneManager.h"

Scene& SceneManager::AddNewScene()
{
	m_Scenes.emplace_back();
	return m_Scenes[static_cast<int>(m_Scenes.size()) - 1];
}

Scene& SceneManager::GetScene()
{
	return m_Scenes[m_CurrentIndex];
}
Scene& SceneManager::GotoScene(const int index)
{
	m_CurrentIndex = index % static_cast<int>(m_Scenes.size());
	return GetScene();
}
Scene& SceneManager::GotoNextScene()
{
	return GotoScene(m_CurrentIndex + 1);
}
Scene& SceneManager::GotoPreviousScene()
{
	return GotoScene(m_CurrentIndex - 1);
}

SceneManager::SceneManager()
{
	m_Scenes.reserve(10);
	m_Scenes.emplace_back();
}
