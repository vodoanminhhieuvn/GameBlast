#pragma once
#include "Object/Object.h"


class CannonBullet : public Object2D
{
private:
	enum class Direction
	{
		Up,
		Down,
		Right,
		Left,
	};
	bool isFacingRight;
	float horizontalMove;
	float verticalMove;
	int defaultBulletSpeed = 60;
	float flyMultiplier = 3.0f;
	std::shared_ptr<Object2D> explosion;
	int damage = 0;
	Direction direction;

public:
	CannonBullet(int direction, float x = 0, float y = 0);

	virtual void Start() override;
	virtual void Update() override;
	virtual void CreateResources() override;
	//khac nhau gia trigger enter voi collision enter la gi ?
	virtual void OnCollisionEnter(std::shared_ptr<Object2D> object) override;
	virtual void OnTriggerEnter(std::shared_ptr<Object2D> object) override;

	int GetDamage() { return damage; }

private:
	void Flip();
};

