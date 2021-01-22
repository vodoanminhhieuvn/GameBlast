#pragma once
#include "Object/Object.h"

class BossBullet : public Object2D
{
private:
	float runSpeed;
	int damage = 3;
	std::shared_ptr<Object2D> player;

public:
	BossBullet(float x = 0, float y = 0);
	virtual void Start() override;
	virtual void Update() override;
	virtual void CreateResources() override;
	virtual void OnCollisionEnter(std::shared_ptr<Object2D> object) override;

};