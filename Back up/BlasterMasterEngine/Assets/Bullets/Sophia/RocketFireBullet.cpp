#include "d3dpch.h"
#include "RocketFireBullet.h"
#include "Core/SceneManager/SceneManager.h"
#include "Assets/Particles/NormalExplosion.h"
#include "Assets/Characters/Enemy/EnemyList.h"
#include "Assets/Bullets/Sophia/BulletSparkle/RocketSparkle.h"
#include <map>

#include "Core/AudioMixer/AudioMixer.h"

RocketFireBullet::RocketFireBullet(float currentX, float currentY, bool pHorizontal, bool pIsFacingRight)
	: Object2D(currentX, currentY)
{
	name = "Rocket Fire Bullet";

	currentBulletX = currentX;
	currentBulletY = currentY;

	horizontal = pHorizontal;
	isFacingRight = pIsFacingRight;
	tag = Tag::PlayerBullet;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	spriteRenderer = GetComponent<SpriteRenderer>();
	layer = Layer::Projectile;
}

void RocketFireBullet::Start()
{
	rigidbody->velocity.x = 0.0f;
	rigidbody->velocity.y = 0.0f;
	runSpeed = 150.0f;
	horizontalRect = { 73, 30, 79, 34 };
	verticalRect = { 92, 29, 96, 35 };
	boxCollider->size = { 3.0f, 3.0f };
	boxCollider->isTrigger = true;
	rigidbody->bodyType = Rigidbody::BodyType::Dynamic;
	rigidbody->gravityScale = 0.0f;
	damage = 3;

	if (isFacingRight)
	{
		transform->Scale(-WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
		rocketSparkle->SetIsFacingRight(TRUE);
	}
	else
	{
		transform->Scale(WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
		rocketSparkle->SetIsFacingLeft(TRUE);
	}

	if (!horizontal)
	{
		rocketSparkle->SetIsFacingDown(TRUE);
		transform->Scale(WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
	}

	//? We get the list of current object rendered inside the camera
	std::list<std::shared_ptr<Object2D>> returnObjects;
	SceneManager::GetActiveScene()->RetrieveRenderableObjects(returnObjects);

	//@param to find the nearest enemy
	long nearestEnemyDistance = 999999;

	for (auto const& object : returnObjects)
	{
		if (object->tag == Tag::Enemy)
		{
			const long currentEnemyX = object->transform->position.x;
			const long currentEnemyY = object->transform->position.y;

			const long currentEnemyDistance = sqrt((pow(currentBulletX - currentEnemyX, 2) + pow(currentBulletY - currentEnemyY, 2) * 1.0));

			if (nearestEnemyDistance > currentEnemyDistance)
			{
				nearestEnemyDistance = currentEnemyDistance;
				nearestEnemy = object;
			}
		}
	}
}

void RocketFireBullet::Update()
{
	std::list<std::shared_ptr<Object2D>> returnObjects;
	SceneManager::GetActiveScene()->RetrieveRenderableObjects(returnObjects);

	std::list<std::shared_ptr<Object2D>>::iterator iteratorReturnObjects;

	if (nearestEnemy)
	{
		iteratorReturnObjects = std::find(returnObjects.begin(), returnObjects.end(), nearestEnemy);

		if (iteratorReturnObjects != returnObjects.end())
		{

			// spriteRenderer->rect = verticalRect;
			if (transform->position.x - nearestEnemy->transform->position.x > 1)
			{
				OnFollowEnemyX(TRUE);

			}
			else if (transform->position.x - nearestEnemy->transform->position.x < -1)
			{
				OnFollowEnemyX(FALSE);

			}
			else
			{

				if (transform->position.y - nearestEnemy->transform->position.y > 0)
				{
					OnFollowEnemyY(TRUE);
				}
				else if (transform->position.y - nearestEnemy->transform->position.y < 0)
				{
					OnFollowEnemyY(FALSE);

				}
			}
		}
		else
		{
			Explode();
		}
	}
	else
	{
		if (horizontal)
		{
			spriteRenderer->rect = horizontalRect;
			rigidbody->velocity.x = (isFacingRight ? 1.0f : -1.0f) * runSpeed * Time::GetFixedDeltaTime();
		}
		else
		{
			spriteRenderer->rect = verticalRect;
			rigidbody->velocity.y = runSpeed * Time::GetFixedDeltaTime();
		}

	}
}

void RocketFireBullet::OnFollowEnemyX(boolean Direction)
{
	if (Time::GetTime() - currentBulletTime >= changeDirectionDelayX)
	{
		switch (Direction)
		{
		case TRUE:
		{
			spriteRenderer->rect = horizontalRect;
			transform->Scale(WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
			currentBulletTime = Time::GetTime();
			rocketSparkle->SetIsFacingLeft(TRUE);
			isFacingRight = false;
			break;
		}
		case FALSE:
		{
			spriteRenderer->rect = horizontalRect;
			transform->Scale(-WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
			currentBulletTime = Time::GetTime();
			rocketSparkle->SetIsFacingRight(TRUE);
			isFacingRight = true;
			break;
		}
		default: return;
		}
	}
	rigidbody->velocity.x = (Direction ? -runSpeed : runSpeed) * Time::GetFixedDeltaTime();
	rigidbody->velocity.y = 0;
}

void RocketFireBullet::OnFollowEnemyY(boolean Direction)
{
	if (Time::GetTime() - currentBulletTimeY >= changeDirectionDelayY)
	{
		switch (Direction)
		{
		case TRUE:
		{
			rocketSparkle->SetIsFacingTop(TRUE);
			spriteRenderer->rect = verticalRect;
			transform->Scale(WINDOW_CAMERA_SCALE_X, -WINDOW_CAMERA_SCALE_Y, 0.0f);
			currentBulletTimeY = Time::GetTime();
			break;
		}
		case FALSE:
		{
			rocketSparkle->SetIsFacingDown(TRUE);
			spriteRenderer->rect = verticalRect;
			transform->Scale(WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
			currentBulletTimeY = Time::GetTime();
			break;
		}
		default: return;
		}
	}
	rigidbody->velocity.x = 0;
	rigidbody->velocity.y = (Direction ? -runSpeed : runSpeed) * Time::GetFixedDeltaTime();
}

void RocketFireBullet::CreateResources()
{
	float scaleX = WINDOW_CAMERA_SCALE_X;
	float scaleY = WINDOW_CAMERA_SCALE_Y;

	{
		rocketSparkle = std::make_shared<RocketSparkle>(0, 0, false, isFacingRight, false, false);
		rocketSparkle->CreateResources();
		rocketSparkle->parentObject = shared_from_this();
		childObjects.emplace_back(rocketSparkle);
	}

	spriteRenderer->sprite = SpriteResources::GetSprite("Sophia_Jason_Texture");
	AudioMixer::PlayWaveFile("SOPHIA_ROCKET", FALSE);
}

void RocketFireBullet::OnTriggerEnter(std::shared_ptr<Object2D> object)
{
	if (object->tag == Tag::Enemy)
	{
		AudioMixer::PlayWaveFile("ENEMY_DIE_NORMAL", FALSE);
		
		std::shared_ptr<EnemyCrab> crab = std::dynamic_pointer_cast<EnemyCrab>(object);
		if (crab != NULL)
		{
			crab->Die();
		}

		std::shared_ptr<EnemyBee> bee = std::dynamic_pointer_cast<EnemyBee>(object);
		if (bee != NULL)
		{
			bee->Die();
		}

		std::shared_ptr<EnemyBulletHead> bhead = std::dynamic_pointer_cast<EnemyBulletHead>(object);
		if (bhead != NULL)
		{
			bhead->Die();
		}

		std::shared_ptr<EnemyCrazyLadybug> clb = std::dynamic_pointer_cast<EnemyCrazyLadybug>(object);
		if (clb != NULL)
		{
			clb->Die();
		}

		std::shared_ptr<EnemyLadybug> lb = std::dynamic_pointer_cast<EnemyLadybug>(object);
		if (lb != NULL)
		{
			lb->Die();
		}

		std::shared_ptr<EnemyLegRobot> lr = std::dynamic_pointer_cast<EnemyLegRobot>(object);
		if (lr != NULL)
		{
			lr->Die();
		}

		std::shared_ptr<EnemySkull> s = std::dynamic_pointer_cast<EnemySkull>(object);
		if (s != NULL)
		{
			s->Die();
		}

		std::shared_ptr<EnemyWorm> w = std::dynamic_pointer_cast<EnemyWorm>(object);
		if (w != NULL)
		{
			w->Die();
		}
	}
	Explode();
}

void RocketFireBullet::Explode()
{
	std::shared_ptr<Object2D> explosion = std::make_shared<NormalExplosion>(transform->position.x, transform->position.y);
	explosion->CreateResources();
	SceneManager::Instantiate(explosion, transform->position);
	SceneManager::DestroyObject(shared_from_this());
}