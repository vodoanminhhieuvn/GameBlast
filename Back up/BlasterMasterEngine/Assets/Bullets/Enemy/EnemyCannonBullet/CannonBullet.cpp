#include "d3dpch.h"
#include "CannonBullet.h"
#include "Core/SceneManager/SceneManager.h"
#include "Assets/Particles/NormalExplosion.h"
#include "Assets/Characters/Sophia/Sophia.h"
#include "Assets/Characters/Jason/Jason.h"
#include <Assets\Characters\Jason\OverworldJason.h>

CannonBullet::CannonBullet(int mDirection, float x, float y)
	: Object2D(x, y)
{
	if (mDirection == 0) direction = Direction::Up;
	if (mDirection == 1) direction = Direction::Down;
	if (mDirection == 2) direction = Direction::Right;
	if (mDirection == 3) direction = Direction::Left;
	name = "CannonBullet";
	tag = Tag::EnemyBullet;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	animationController = GetComponent<AnimationController>();
	spriteRenderer = GetComponent<SpriteRenderer>();
}

void CannonBullet::CreateResources()
{
	spriteRenderer->sprite = SpriteResources::GetSprite("Enemy_Bullet_Texture");

	int spriteWidth = 6;
	int spriteHeight = 6;
	KeyFrame keyFrame;
	RECT rect;
	keyFrame.position = transform->position;
	keyFrame.scale = { 1.0f, 1.0f, 0.0f };

	std::shared_ptr<Animation> bulletFly = std::make_shared<Animation>("Bullet Fly");
	{
		bulletFly->SetAnimationFPS(10);

		rect.left = 360;
		rect.top = 164;
		rect.right = rect.left + spriteWidth;
		rect.bottom = rect.top + spriteHeight;

		keyFrame.rect = rect;
		bulletFly->AddKeyFrames(keyFrame);
		animationController->AddAnimation(bulletFly);
		animationController->SetDefaultAnimation(bulletFly);
	}
}

void CannonBullet::Start()
{
	switch (direction)
	{
	case Direction::Up:
		horizontalMove = 0.0f;
		verticalMove = 40.0f;
		break;
	case Direction::Down:
		horizontalMove = 0.0f;
		verticalMove = -40.0f;
		break;
	case Direction::Right:
		horizontalMove = 40.0f;
		verticalMove = 0.0f;
		break;
	case Direction::Left:
		horizontalMove = -40.0f;
		verticalMove = 0.0f;
		break;
	}
	isFacingRight = true;
	rigidbody->bodyType = Rigidbody::BodyType::Dynamic;
	rigidbody->gravityScale = 0.0f;
	rigidbody->bounciness = 0.0f;
	boxCollider->size = { 6.0f, 6.0f };
	boxCollider->offset = { 0.0f, 0.0f };
	boxCollider->isTrigger = false;
	transform->scale = { 3.0f, 3.0f, 3.0f };
}

void CannonBullet::Update()
{
	rigidbody->velocity.x = horizontalMove * flyMultiplier * Time::GetFixedDeltaTime();
	rigidbody->velocity.y = verticalMove * flyMultiplier * Time::GetFixedDeltaTime();
}

void CannonBullet::OnTriggerEnter(std::shared_ptr<Object2D> object)
{

}

void CannonBullet::OnCollisionEnter(std::shared_ptr<Object2D> object)
{
	if (object->tag == Tag::Player && object->rigidbody->bodyType == Rigidbody::BodyType::Dynamic)
	{
		std::shared_ptr<OverworldJason> jason = std::dynamic_pointer_cast<OverworldJason>(object);
		if (jason != NULL)
		{
			jason->TakeDamage(damage);
		}

	}

	std::shared_ptr<Object2D> explosion = std::make_shared<NormalExplosion>(transform->position.x, transform->position.y);
	explosion->CreateResources();
	SceneManager::Instantiate(explosion, transform->position);
	SceneManager::DestroyObject(shared_from_this());
}

void CannonBullet::Flip()
{
}