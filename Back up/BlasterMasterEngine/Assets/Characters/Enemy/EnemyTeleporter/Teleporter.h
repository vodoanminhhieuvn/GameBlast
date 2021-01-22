#pragma once
#include "Object/OrthographicCamera/OrthographicCamera.h"
#include "Object/Object.h"

class Teleporter : public Object2D
{
private:

	int intervalAttack;
	int hitPoint;
	float detectZoneX;
	float detectZoneY;
	float horizontalMove;
	float verticalMove;
	float transformAnimationDelay;
	float transformCurrentTime;
	float attackStateCurrentTime;
	float attackCurrentTime;
	float teleportDistance;
	float attackStateDelay;
	float attackDelay;
	float teleTime;
	float teleCurrent;
	bool isDetect;
	bool isAttacking;
	bool isAttack;
	bool isTele;

	int damage;

	std::shared_ptr<Object2D> bullet;
	std::shared_ptr<Object2D> player;

public:
	Teleporter(float x = 0, float y = 0);

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
	void Tele();
	void Attack();
	void TranslateDirection(float& x, float& y);
};
