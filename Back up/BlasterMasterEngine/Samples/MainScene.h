#pragma once
#include "Core/SceneManager/Scene.h"

class MainScene : public Scene
{
public:
	MainScene()
	{
		name = "MainScene";
	}

	~MainScene() {}

	virtual void CreateScene() override;
};