#pragma once
#include "Object/Object.h"

class EnemyGuardian : public Object2D
{
private:


	Direction direction;
	float runSpeed;
	bool isRightMove;
	bool isLeftMove;
	bool isDownMove;
	bool isUpMove;
	bool isNearPlayer;
	float detectingPlayerZoneX = 50.0f;
	float detectingPlayerZoneY = 50.0f;
	float changeDirectionDelay = 500.0f;
	float changeDirectionTime;
	float shootTime;
	float delayShoot = 2000.0f;
	int defaultFPS = 6;
	int damage = 0;
	std::shared_ptr<Object2D> player;

public:
	EnemyGuardian(float x = 0, float y = 0, float xMove = 45.0f);

	virtual void Start() override;
	virtual void Update() override;
	virtual void CreateResources() override;
	virtual void OnCollisionStay(std::shared_ptr<Object2D> object) override;
	virtual void OnTriggerEnter(std::shared_ptr<Object2D> object) override;

	int GetDamage() { return damage; }
	void Die();

private:
	void SetAnimationParameter();
	void Shoot();

};


