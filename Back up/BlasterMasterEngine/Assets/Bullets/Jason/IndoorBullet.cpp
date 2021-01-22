#include "d3dpch.h"
#include "IndoorBullet.h"

#include <Assets/Bullets/Enemy/BossBullet/BossBullet.h>

#include "Core/SceneManager/SceneManager.h"
#include "Assets/Particles/NormalExplosion.h"
#include "Assets/Characters/Enemy/EnemyList.h"
#include <Assets\Characters\Enemy\EnemyCannon\Cannon.h>
#include <Assets\Characters\Enemy\EnemyTeleporter\Teleporter.h>
#include <Assets\Characters\Enemy\EnemyEyeball\Eyeball.h>



#include "Assets/Characters/Enemy/BossArea2/BossPart.h"
#include "Assets/Characters/Enemy/EnemyGuardian/EnemyGuardian.h"
#include "Assets/Characters/Enemy/EnemyWalker/EnemyWalker.h"
#include "Core/AudioMixer/AudioMixer.h"
//#include <Teleporter.h>
//#include <Eyeball.h>
//#include <Cannon.h>

IndoorBullet::IndoorBullet(int mDirection, float x, float y)
	: Object2D(x, y)
{
	name = "Normal Fire Bullet";
	if (mDirection == 0) direction = Direction::Up;
	if (mDirection == 1) direction = Direction::Down;
	if (mDirection == 2) direction = Direction::Right;
	if (mDirection == 3) direction = Direction::Left;
	tag = Tag::PlayerBullet;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	spriteRenderer = GetComponent<SpriteRenderer>();
	layer = Layer::Projectile;
}


void IndoorBullet::Start()
{
	runSpeed = 150.0f;
	horizontalRect = { 73, 20, 81, 28 };
	verticalRect = { 82, 20, 90, 28 };
	boxCollider->size = { 7.0f, 7.0f };
	boxCollider->isTrigger = true;
	rigidbody->bodyType = Rigidbody::BodyType::Dynamic;
	rigidbody->gravityScale = 0.0f;
	damage = 3;
	transform->Scale(isFacingRight ? -WINDOW_CAMERA_SCALE_X : WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
}

void IndoorBullet::Update()
{

	switch (direction)
	{
	case Direction::Up:
		spriteRenderer->rect = verticalRect;
		rigidbody->velocity.x = 0.0f;
		rigidbody->velocity.y = 1.0f * runSpeed * Time::GetFixedDeltaTime();
		transform->Scale(3.0f, -3.0f, 0.0f);
		break;
	case Direction::Down:
		spriteRenderer->rect = verticalRect;
		rigidbody->velocity.x = 0.0f;
		rigidbody->velocity.y = -1.0f * runSpeed * Time::GetFixedDeltaTime();
		transform->Scale(3.0f, 3.0f, 0.0f);
		break;
	case Direction::Right:
		spriteRenderer->rect = horizontalRect;
		rigidbody->velocity.x = 1.0f * runSpeed * Time::GetFixedDeltaTime();
		rigidbody->velocity.y = 0.0;
		transform->Scale(-3.0f, 3.0f, 0.0f);
		break;
	case Direction::Left:
		spriteRenderer->rect = horizontalRect;
		rigidbody->velocity.x = -1.0f * runSpeed * Time::GetFixedDeltaTime();
		rigidbody->velocity.y = 0.0;
		transform->Scale(3.0f, 3.0f, 0.0f);
		break;
	}
}

void IndoorBullet::CreateResources()
{
	spriteRenderer->sprite = DeviceResources::LoadTexture(SOPHIA_JASON_TEXTURE_PATH, 0);
}

void IndoorBullet::OnTriggerEnter(std::shared_ptr<Object2D> object)
{
	if (object->tag == Tag::Enemy)
	{

		std::shared_ptr<Cannon> cannon = std::dynamic_pointer_cast<Cannon>(object);
		if (cannon != NULL)
		{
			cannon->TakeDamage(damage);
		}
		std::shared_ptr<Teleporter> teleporter = std::dynamic_pointer_cast<Teleporter>(object);
		if (teleporter != NULL)
		{
			if (teleporter->animationController->GetCurrentAnimation()->GetName() == "Teleporter Attack")
			{
				teleporter->TakeDamage(damage);
			}
		}
		std::shared_ptr<Eyeball> eyeball = std::dynamic_pointer_cast<Eyeball>(object);
		if (eyeball != NULL)
		{
			eyeball->TakeDamage(damage);
		}
		std::shared_ptr<EnemyWalker> enemyWalker = std::dynamic_pointer_cast<EnemyWalker>(object);
		if (enemyWalker != NULL)
		{
			enemyWalker->Die();
			AudioMixer::PlayWaveFile("ENEMY_DIE_NORMAL", FALSE);
		}

		std::shared_ptr<EnemyGuardian> enemyGuardian = std::dynamic_pointer_cast<EnemyGuardian>(object);
		if (enemyGuardian != NULL)
		{
			enemyGuardian->Die();
			AudioMixer::PlayWaveFile("ENEMY_DIE_NORMAL", FALSE);
		}

		if (object->tag == Tag::Boss && std::dynamic_pointer_cast<BossPart>(object) != nullptr)
		{
			// if(std::dynamic_pointer_cast<BossArea2>(object)->GetIsAttack())
			// {
			// 	AudioMixer::PlayWaveFile("BOSS_HIT_1", false);
			// }
			std::dynamic_pointer_cast<BossPart>(object)->TakeDamage(damage);
		}
	}
	Explode();
	
	if (object->tag == Tag::EnemyBullet && std::dynamic_pointer_cast<BossBullet>(object) != nullptr)
	{
		SceneManager::DestroyObject(object);
	}
}

void IndoorBullet::Explode()
{
	std::shared_ptr<Object2D> explosion = std::make_shared<NormalExplosion>(transform->position.x, transform->position.y);
	explosion->CreateResources();
	SceneManager::Instantiate(explosion, transform->position);
	SceneManager::DestroyObject(shared_from_this());
}
