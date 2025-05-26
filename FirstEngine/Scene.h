#pragma once
#include "CPP/GameObject.h"
#include <vector>

class Scene
{
public:
    Scene();
    ~Scene();

    void AddGameObject(GameObject* gameObject);
    void RemoveGameObject(GameObject* gameObject);
    void Update(float deltaTime);

    std::vector<GameObject*> gameObjects;
};

