#include "d3dpch.h"
#include "TeleporterBullet.h"
#include "Core/SceneManager/SceneManager.h"
#include "Assets/Particles/NormalExplosion.h"
#include "Assets/Characters/Sophia/Sophia.h"
#include "Assets/Characters/Jason/Jason.h"
#include <Assets\Characters\Jason\OverworldJason.h>

TeleporterBullet::TeleporterBullet(float dx, float dy, float x, float y)
	: Object2D(x, y)
{
	directionX = dx;
	directionY = dy;
	name = "TeleporterBullet";
	tag = Tag::EnemyBullet;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	animationController = GetComponent<AnimationController>();
	spriteRenderer = GetComponent<SpriteRenderer>();
}

void TeleporterBullet::CreateResources()
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

void TeleporterBullet::Start()
{
	horizontalMove = 40.0f;
	verticalMove = 40.0f;
	isFacingRight = true;
	rigidbody->bodyType = Rigidbody::BodyType::Dynamic;
	rigidbody->gravityScale = 0.0f;
	rigidbody->bounciness = 0.0f;
	boxCollider->size = { 6.0f, 6.0f };
	boxCollider->offset = { 0.0f, 0.0f };
	boxCollider->isTrigger = false;
	transform->scale = { 3.0f, 3.0f, 3.0f };
}

void TeleporterBullet::Update()
{
	rigidbody->velocity.x = directionX * horizontalMove * flyMultiplier * Time::GetFixedDeltaTime();
	rigidbody->velocity.y = directionY * verticalMove * flyMultiplier * Time::GetFixedDeltaTime();
}

void TeleporterBullet::OnTriggerEnter(std::shared_ptr<Object2D> object)
{

}

void TeleporterBullet::OnCollisionEnter(std::shared_ptr<Object2D> object)
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

void TeleporterBullet::Flip()
{
}