#pragma once
#include "Object/Object.h"

class IndoorBullet : public Object2D
{
private:
	enum class Direction
	{
		Up,
		Down,
		Right,
		Left,
	};
	RECT horizontalRect;
	RECT verticalRect;
	bool horizontal;
	bool isFacingRight;
	float runSpeed;

	int damage = 3;
	Direction direction;

public:
	IndoorBullet(int direction, float x = 0, float y = 0);
	virtual void Start() override;
	virtual void Update() override;
	virtual void CreateResources() override;
	virtual void OnTriggerEnter(std::shared_ptr<Object2D> object) override;


private:
	void Explode();
};