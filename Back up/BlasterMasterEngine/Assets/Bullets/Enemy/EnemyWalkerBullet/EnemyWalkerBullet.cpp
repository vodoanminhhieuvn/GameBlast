#include "d3dpch.h"
#include "EnemyWalkerBullet.h"

#include "Core/SceneManager/SceneManager.h"
#include "Assets/Particles/NormalExplosion.h"
#include "Assets/Characters/Sophia/Sophia.h"
#include "Assets/Characters/Jason/Jason.h"
#include "Assets/Characters/Jason/OverworldJason.h"

EnemyWalkerBullet::EnemyWalkerBullet(float x, float y, Direction direction, bool isLeftAnimation)
	: Object2D(x, y)
{
	directionBullet = direction;
	name = "EnemyWalkerBullet";
	tag = Tag::EnemyBullet;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	animationController = GetComponent<AnimationController>();
	spriteRenderer = GetComponent<SpriteRenderer>();

	this->isLeftAnimation = isLeftAnimation;
}

void EnemyWalkerBullet::CreateResources()
{
	spriteRenderer->sprite = SpriteResources::GetSprite("Enemy_Texture");

	int spriteWidth = 8;
	int spriteHeight = 8;
	KeyFrame keyFrame;
	RECT rect;
	keyFrame.scale = { 1.0f, 1.0f, 0.0f };

	if (isLeftAnimation)
	{
		std::shared_ptr<Animation> leftBulletWalker = std::make_shared<Animation>("Left Bullet Walker");
		{
			leftBulletWalker->SetAnimationFPS(10);

			rect.left = 214;
			rect.top = 305;
			rect.right = rect.left + spriteWidth;
			rect.bottom = rect.top + spriteHeight;

			keyFrame.rect = rect;
			leftBulletWalker->AddKeyFrames(keyFrame);
			animationController->AddAnimation(leftBulletWalker);
			animationController->SetDefaultAnimation(leftBulletWalker);
		}
	}
	
	else
	{
		std::shared_ptr<Animation> rightBulletWalker = std::make_shared<Animation>("Right Bullet Walker");
		{
			rightBulletWalker->SetAnimationFPS(10);

			rect.left = 32;
			rect.top = 305;
			rect.right = rect.left + spriteWidth;
			rect.bottom = rect.top + spriteHeight;

			keyFrame.rect = rect;
			rightBulletWalker->AddKeyFrames(keyFrame);
			animationController->AddAnimation(rightBulletWalker);
			animationController->SetDefaultAnimation(rightBulletWalker);
		}
	}
}

void EnemyWalkerBullet::Start()
{
	rigidbody->bodyType = Rigidbody::BodyType::Dynamic;
	rigidbody->gravityScale = 0.0f;
	rigidbody->bounciness = 0.0f;
	boxCollider->size = { 8.0f, 8.0f };
	boxCollider->offset = { 0.0f, 0.0f };
	boxCollider->isTrigger = false;
	transform->scale = { WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f };
	if (directionBullet == Direction::LEFT)
	{
		rigidbody->velocity.x = -defaultBulletSpeed * Time::GetFixedDeltaTime();
		rigidbody->velocity.y = 0;
	}
	else if (directionBullet == Direction::RIGHT)
	{
		rigidbody->velocity.x = defaultBulletSpeed * Time::GetFixedDeltaTime();
		rigidbody->velocity.y = 0;
	}
	else if (directionBullet == Direction::UP)
	{
		rigidbody->velocity.x = 0;
		rigidbody->velocity.y = defaultBulletSpeed * Time::GetFixedDeltaTime();
	}
	else if (directionBullet == Direction::DOWN)
	{
		rigidbody->velocity.x = 0;
		rigidbody->velocity.y = -defaultBulletSpeed * Time::GetFixedDeltaTime();
	}
}

void EnemyWalkerBullet::Update()
{
}

void EnemyWalkerBullet::OnTriggerEnter(std::shared_ptr<Object2D> object)
{
}

void EnemyWalkerBullet::OnCollisionEnter(std::shared_ptr<Object2D> object)
{
	if (object->tag == Tag::Player && object->rigidbody->bodyType == Rigidbody::BodyType::Dynamic)
	{
		std::shared_ptr<OverworldJason> overworldJason = std::dynamic_pointer_cast<OverworldJason>(object);
		if (overworldJason != NULL)
		{
			overworldJason->TakeDamage(damage);
			SceneManager::DestroyObject(shared_from_this());
		}
	}

	std::shared_ptr<Object2D> explosion = std::make_shared<NormalExplosion>(transform->position.x, transform->position.y);
	explosion->CreateResources();
	SceneManager::Instantiate(explosion, transform->position);
	SceneManager::DestroyObject(shared_from_this());
}
