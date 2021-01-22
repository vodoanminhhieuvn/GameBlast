#include "d3dpch.h"
#include "BossBullet.h"

#include "Assets/Characters/Jason/OverworldJason.h"
#include "Core/SceneManager/SceneManager.h"
#include "Assets/Particles/NormalExplosion.h"


BossBullet::BossBullet(float x, float y)
	: Object2D(x, y)
{
	name = "Boss Fire Bullet";
	tag = Tag::EnemyBullet;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	spriteRenderer = GetComponent<SpriteRenderer>();
	layer = Layer::Projectile;
}

void BossBullet::Start()
{
	runSpeed = 150.0f;
	boxCollider->size = { 9.0f, 9.0f };
	boxCollider->isTrigger = true;
	rigidbody->bodyType = Rigidbody::BodyType::Dynamic;
	rigidbody->gravityScale = 0.0f;
	damage = 1;
	transform->Scale(WINDOW_CAMERA_SCALE_X, WINDOW_CAMERA_SCALE_Y, 0.0f);
	spriteRenderer->rect = { 58,62,67,71 };

	player = SceneManager::GetActiveScene()->FinObjectByName("OverworldJason");
	D3DXVECTOR2 distanceToPlayer;
	distanceToPlayer.x = player->transform->position.x - transform->position.x;
	distanceToPlayer.y = player->transform->position.y - transform->position.y;

	int totalVelocity = sqrt(pow(distanceToPlayer.x, 2) + pow(distanceToPlayer.y, 2));
	if (totalVelocity == 0) totalVelocity = 1;
	rigidbody->velocity.x = distanceToPlayer.x * Time::GetFixedDeltaTime() * (runSpeed / totalVelocity);
	rigidbody->velocity.y = -abs(distanceToPlayer.y) * Time::GetFixedDeltaTime() * (runSpeed / totalVelocity);
}

void BossBullet::Update()
{
}

void BossBullet::CreateResources()
{
	spriteRenderer->sprite = DeviceResources::LoadTexture(ENEMY_BULLET_TEXTURE_PATH, 0);
}

void BossBullet::OnCollisionEnter(std::shared_ptr<Object2D> object)
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
}
