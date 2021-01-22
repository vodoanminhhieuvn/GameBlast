#include "d3dpch.h"
#include "EnemyRedWalker.h"

// #include "Assets/Bullets/Enemy/EnemyRedWalkerBullet/EnemyRedWalkerBullet.h"
#include "Assets/Bullets/Enemy/EnemyWalkerBullet/EnemyWalkerBullet.h"
#include "Core/SceneManager/SceneManager.h"
#include "Assets/Characters/Sophia/Sophia.h"
#include "Assets/Characters/Jason/Jason.h"


EnemyRedWalker::EnemyRedWalker(float x, float y, float RunSpeed)
	: EnemyWalker(x, y)
{
	name = "Enemy Red Walker";
	tag = Tag::Enemy;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	animationController = GetComponent<AnimationController>();
	spriteRenderer = GetComponent<SpriteRenderer>();
	runSpeed = RunSpeed;
}

void EnemyRedWalker::CreateResources()
{
#pragma region animation
	spriteRenderer->sprite = SpriteResources::GetSprite("Enemy_Texture");

	int spriteWidth = 18;
	int spriteHeight = 17;
	KeyFrame keyFrame;
	RECT rect;
	keyFrame.scale = { 1.0f, 1.0f, 0.0f };


	std::shared_ptr<Animation> IdleAnimation = std::make_shared<Animation>("Idle");
	{
		IdleAnimation->SetAnimationFPS(defaultFPS);

		rect.left = 84;
		rect.top = 1150;
		rect.right = rect.left + spriteWidth;
		rect.bottom = rect.top + spriteHeight;
		keyFrame.rect = rect;
		IdleAnimation->AddKeyFrames(keyFrame);

		animationController->AddAnimation(IdleAnimation);
		animationController->SetDefaultAnimation(IdleAnimation);
	}

	std::shared_ptr<Animation> AttackAnimation = std::make_shared<Animation>("Attack");
	{
		AttackAnimation->SetAnimationFPS(defaultFPS);
		for (int index = 0; index < 2; index++)
		{
			rect.left = 84 + (spriteWidth + 2) * index;
			rect.top = 1150;
			rect.right = rect.left + spriteWidth;
			rect.bottom = rect.top + spriteHeight;
			keyFrame.rect = rect;
			AttackAnimation->AddKeyFrames(keyFrame);
		}
		animationController->AddAnimation(AttackAnimation);
	}

#pragma endregion

#pragma region transition
	std::shared_ptr<Parameter<bool>> isIdle = std::make_shared<Parameter<bool>>("isIdle");
	{
		animationController->AddBoolParameter(isIdle);
	}

	std::shared_ptr<TransitionCondition<bool>> isIdleBoolCond = std::make_shared<TransitionCondition<bool>>();
	{
		isIdleBoolCond->SetParameter(isIdle);
		isIdleBoolCond->SetValue(true);
	}

	std::shared_ptr<TransitionCondition<bool>> isAttackCond = std::make_shared<TransitionCondition<bool>>();
	{
		isAttackCond->SetParameter(isIdle);
		isAttackCond->SetValue(false);
	}

	std::shared_ptr<Transition> idleToAttack = std::make_shared<Transition>(
		animationController->GetAnimationIndex(IdleAnimation),
		animationController->GetAnimationIndex(AttackAnimation));
	{
		idleToAttack->AddBoolTransitionCondition(isAttackCond);
		animationController->AddTransition(idleToAttack);
	}

	std::shared_ptr<Transition> attackToIdle = std::make_shared<Transition>(
		animationController->GetAnimationIndex(AttackAnimation),
		animationController->GetAnimationIndex(IdleAnimation));
	{
		attackToIdle->AddBoolTransitionCondition(isIdleBoolCond);
		animationController->AddTransition(attackToIdle);
	}
#pragma endregion
}

void EnemyRedWalker::Start()
{
	EnemyWalker::Start();
	shootTime = 0.0f;
}

void EnemyRedWalker::Update()
{
	//DETECTING PLAYER
	player = SceneManager::GetActiveScene()->FinObjectByName("OverworldJason");
	if (abs((transform->position.x) - player->transform->position.x) <= detectingPlayerZoneX && abs((transform->position.y) - player->transform->position.y) <= detectingPlayerZoneY)
	{
		isNearPlayer = true;
	}
	else
	{
		isNearPlayer = false;
	}

	//MOVEMENT
	if (isNearPlayer == true)
	{
		isIdle = FALSE;

		if (Time::GetTime() - changeDirectionTime >= changeDirectionDelay)
		{
			float distanceX = abs(transform->position.x - player->transform->position.x);
			float distanceY = abs(transform->position.y - player->transform->position.y);

			if (!(transform->position.x - player->transform->position.x < player->boxCollider->size.width / 4 &&
				transform->position.x - player->transform->position.x > -player->boxCollider->size.width / 4))
			{
				rigidbody->velocity.x = (transform->position.x < player->transform->position.x ? 1 : -1) * runSpeed * Time::GetFixedDeltaTime();
			}
			else rigidbody->velocity.x = 0.0f;

			if (!(transform->position.y - player->transform->position.y < player->boxCollider->size.height / 4 &&
				transform->position.y - player->transform->position.y > -player->boxCollider->size.height / 4))
			{
				rigidbody->velocity.y = (transform->position.y < player->transform->position.y ? 1 : -1) * runSpeed * Time::GetFixedDeltaTime();
			}
			else rigidbody->velocity.y = 0.0f;


			// set bool
			if (rigidbody->velocity.x > 0)
			{
				direction = Direction::RIGHT;
			}
			else if (rigidbody->velocity.x < 0)
			{
				direction = Direction::LEFT;
			}

			if (rigidbody->velocity.y > 0)
			{
				direction = Direction::UP;
			}
			else if (rigidbody->velocity.y < 0)
			{
				direction = Direction::DOWN;
			}
			
			changeDirectionTime = Time::GetTime();
		}

		if (Time::GetTime() - shootTime >= delayShoot)
		{
			if (rigidbody->velocity.x == 0 || rigidbody->velocity.y == 0)
			Shoot();
			shootTime = Time::GetTime();
		}
		shootTime += Time::GetFixedDeltaTime();
		changeDirectionTime += Time::GetFixedDeltaTime();
	}
	else
	{
		isIdle = TRUE;
		rigidbody->velocity = { 0.0f,0.0f };
	}
	SetAnimationParameter();

}

void EnemyRedWalker::SetAnimationParameter()
{
	animationController->SetBool("isIdle", isIdle);
}

void EnemyRedWalker::Shoot()
{
	std::shared_ptr<Object2D> bullet = std::make_shared<EnemyWalkerBullet>(transform->position.x, transform->position.y,
		direction, (direction == Direction::RIGHT ? false : true));
	bullet->CreateResources();
	SceneManager::Instantiate(bullet, bullet->transform->position);
}
