#pragma once
#include "Assets/Characters/Enemy/EnemyWalker/EnemyWalker.h"
#include "Object/Object.h"

class EnemyRedWalker : public EnemyWalker
{
private:


	Direction direction;
	float runSpeed;

	bool isIdle;
	bool isNearPlayer;

	float detectingPlayerZoneX = 100.0f;
	float detectingPlayerZoneY = 100.0f;
	float changeDirectionDelay = 500.0f;
	float changeDirectionTime;

	float shootTime;
	float delayShoot = 1000.0f;
	int defaultFPS = 6;
	int damage = 0;
	std::shared_ptr<Object2D> player;

public:
	EnemyRedWalker(float x = 0, float y = 0, float xMove = 45.0f);

	virtual void Start() override;
	virtual void Update() override;
	virtual void CreateResources() override;

private:
	void SetAnimationParameter();
	void Shoot();

};


