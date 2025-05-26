#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for (auto gameObject : gameObjects)
    {
        delete gameObject;
    }
}

void Scene::AddGameObject(GameObject* gameObject)
{
    gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(GameObject* gameObject)
{
    gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
}

void Scene::Update(float deltaTime)
{
    for (auto gameObject : gameObjects)
    {
        gameObject->Update(deltaTime);
    }
}
