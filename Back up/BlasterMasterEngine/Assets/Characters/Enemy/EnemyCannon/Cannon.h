#pragma once
#include "Object/OrthographicCamera/OrthographicCamera.h"
#include "Object/Object.h"

class Cannon : public Object2D
{
private:

	enum class State
	{
		Unseen,
		Trigger,
		Die
	};

	int hitPoint;
	float afterAttackDelayTime;
	float attackingDelayTime;
	float startWaitinAfterAttackgAt;
	float startAttackingAt;
	float attackingDelayRemain;
	float nextAttackIn;
	bool isVerticalAttack;
	bool isTrigger;
	bool isAttacking;
	bool isWaitingAfterAttack;

	std::shared_ptr<Object2D> bullet_1;
	std::shared_ptr<Object2D> bullet_2;
	std::shared_ptr<Object2D> deadExplosion;

	State state;

public:
	Cannon(float x = 0, float y = 0);
	~Cannon()
	{
	}

	virtual void Start() override;
	void Update() override;
	virtual void CreateResources() override;
	virtual void OnCollisionEnter(std::shared_ptr<Object2D> object) override;

	void TakeDamage(int p_Damage);
private:
	void SetAnimationParameter();
	void Die();
	void IsInCamera();
	void AttackCheck();
	void Attack();
};
