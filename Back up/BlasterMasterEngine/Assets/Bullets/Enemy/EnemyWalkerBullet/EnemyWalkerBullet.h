#pragma once
#include "Object/Object.h"


class EnemyWalkerBullet : public Object2D
{
private:

	bool isFacingRight;
	int defaultBulletSpeed = 60;
	Direction directionBullet;
	std::shared_ptr<Object2D> explosion;
	int damage = 0;

	bool isLeftAnimation;

public:
	EnemyWalkerBullet(float x = 0, float y = 0, Direction direction = Direction::UP, bool isLeftAnimation = FALSE);

	virtual void Start() override;
	virtual void Update() override;
	virtual void CreateResources() override;
	virtual void OnCollisionEnter(std::shared_ptr<Object2D> object) override;
	virtual void OnTriggerEnter(std::shared_ptr<Object2D> object) override;

	int GetDamage() { return damage; }

};

