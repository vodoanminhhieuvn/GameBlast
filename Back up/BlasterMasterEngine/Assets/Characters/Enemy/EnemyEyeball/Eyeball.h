#pragma once
#include "Object/OrthographicCamera/OrthographicCamera.h"
#include "Object/Object.h"

class Eyeball : public Object2D
{
private:

	int hitPoint;
	float horizontalMove;
	float verticalMove;
	float testY;
	bool isAttack;
	bool circleUp;
	bool forward;
	float idleStateDelay;
	float attackStateDelay;
	float currentIdleStateTime;
	float currentAttackStateTime;
	float randomDestMaxX;
	float randomDestMaxY;
	float randomDestMinX;
	float randomDestMinY;
	float centerPointX;
	float centerPointY;
	float radiant;
	float radius;
	float destX;
	float destY;
	float PI;

	int damage;

	std::shared_ptr<Object2D> bullet;
	std::shared_ptr<Object2D> player;

public:
	Eyeball(float x = 0, float y = 0);

	virtual void Start() override;
	void Update() override;
	virtual void CreateResources() override;
	virtual void OnCollisionEnter(std::shared_ptr<Object2D> object) override;
	virtual void OnCollisionStay(std::shared_ptr<Object2D> object) override;

	void TakeDamage(int p_Damage);

private:
	void SetAnimationParameter();
	void Die();
	void AttackCheck();
	void Move();
	void Attack();
	void TranslateDirection(float& x, float& y, float posX, float posY);
	void TranslateDirection(float& x, float& y);
	void SetTrajectory();
};
