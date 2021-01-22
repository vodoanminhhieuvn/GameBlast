#include "d3dpch.h"
#include "Eyeball.h"
#include "Core/SceneManager/SceneManager.h"
#include "Assets/CharacterController.h"
#include "Assets/CameraBound.h"
#include "Assets/Particles/PlayerDeadExplosion.h"
#include <Assets\Bullets\Enemy\EnemyBullet.h>
#include <Assets\Characters\Jason\OverworldJason.h>
#include <Assets\Bullets\Enemy\EnemyTeleporterBullet\TeleporterBullet.h>

Eyeball::Eyeball(float x, float y)
	: Object2D(x, y)
{
	name = "Eyeball";
	tag = Tag::Enemy;
	rigidbody = GetComponent<Rigidbody>();
	boxCollider = GetComponent<BoxCollider2D>();
	spriteRenderer = GetComponent<SpriteRenderer>();
	animationController = GetComponent<AnimationController>();
	layer = Layer::Enemy;
}

void Eyeball::CreateResources()
{
	spriteRenderer->sprite = DeviceResources::LoadTexture(ENEMY_TEXTURE_PATH, 0);

	int spriteWidth = 16;
	int spriteHeight = 16;

	KeyFrame keyFrame;
	RECT rect = { 0, 0, 0, 0 };
	keyFrame.scale = { 1.0f, 1.0f, 0.0f };

	std::shared_ptr<Animation> eyeballIdle = std::make_shared<Animation>("Eyeball Idle");
	{
		eyeballIdle->SetAnimationFPS(6);

		rect.left = 119;
		rect.top = 550;
		rect.right = rect.left + spriteWidth;
		rect.bottom = rect.top + spriteHeight;
		keyFrame.rect = rect;

		eyeballIdle->AddKeyFrames(keyFrame);

		animationController->AddAnimation(eyeballIdle);

		animationController->SetDefaultAnimation(eyeballIdle);
	}

	std::shared_ptr<Animation> eyeballAttack = std::make_shared<Animation>("Eyeball Attack");
	{
		eyeballAttack->SetAnimationFPS(12);

		for (size_t index = 0; index < 2; index++)
		{
			rect.left = 102 + (int)index * 34;
			rect.top = 550;
			rect.right = rect.left + spriteWidth;
			rect.bottom = rect.top + spriteHeight;
			keyFrame.rect = rect;
			eyeballAttack->AddKeyFrames(keyFrame);
		}

		animationController->AddAnimation(eyeballAttack);
	}

	std::shared_ptr<Parameter<bool>> isAttack = std::make_shared<Parameter<bool>>("isAttack");
	{
		animationController->AddBoolParameter(isAttack);
	}

	std::shared_ptr<TransitionCondition<bool>> isAttackTrueBoolCond = std::make_shared<TransitionCondition<bool>>();
	{
		isAttackTrueBoolCond->SetParameter(isAttack);
		isAttackTrueBoolCond->SetValue(true);
	}

	std::shared_ptr<TransitionCondition<bool>> isAttackFalseBoolCond = std::make_shared<TransitionCondition<bool>>();
	{
		isAttackFalseBoolCond->SetParameter(isAttack);
		isAttackFalseBoolCond->SetValue(false);
	}

	//tan cong xong dung yen
	std::shared_ptr<Transition> attackingToIdleTrans = std::make_shared<Transition>(
		animationController->GetAnimationIndex(eyeballAttack),
		animationController->GetAnimationIndex(eyeballIdle));
	{
		attackingToIdleTrans->AddBoolTransitionCondition(isAttackFalseBoolCond);
		animationController->AddTransition(attackingToIdleTrans);
	}

	//dung yen xong tan cong
	std::shared_ptr<Transition> idleToAttackTrans = std::make_shared<Transition>(
		animationController->GetAnimationIndex(eyeballIdle),
		animationController->GetAnimationIndex(eyeballAttack));
	{
		idleToAttackTrans->AddBoolTransitionCondition(isAttackTrueBoolCond);
		animationController->AddTransition(idleToAttackTrans);
	}
}

void Eyeball::Start()
{
	srand(time(NULL));
	rigidbody->bodyType = Rigidbody::BodyType::Dynamic;
	rigidbody->bounciness = 0.0f;
	boxCollider->size = { 16.0f, 16.0f };
	boxCollider->offset = { 0.0f, 0.0f };
	boxCollider->isTrigger = false;
	attackStateDelay = 3.0f;
	hitPoint = 3;
	damage = 0;
	transform->scale = { -3.0f, 3.0f, 3.0f };
	rigidbody->gravityScale = 0.0f;
	isAttack = false;
	idleStateDelay = 0.5f;
	attackStateDelay = 2.0f;
	currentIdleStateTime = 0.0f;
	currentAttackStateTime = 0.0f;
	randomDestMaxX = 60.0f;
	randomDestMaxY = 60.0f;
	randomDestMinX = 30.0f;
	randomDestMinY = 30.0f;
	PI = 3.1415;
	destX = 0;
	destY = 0;
}

void Eyeball::Update()
{
	testY = transform->position.y;
	//LOG_INFO("{0}", animationController->GetCurrentAnimation()->GetName());
	AttackCheck();
	SetAnimationParameter();
}

void Eyeball::AttackCheck()
{
	if (isAttack == false)
	{
		currentIdleStateTime += Time::GetDeltaTime();
		if (currentIdleStateTime >= idleStateDelay)
		{
			isAttack = true;
			Attack();
			SetTrajectory();
			currentIdleStateTime = 0.0f;
		}
		/*std::string sString = "current idle state :" + std::to_string(currentIdleStateTime);
		LOG_INFO(sString);*/
	}
	if (isAttack == true)
	{
		Move();
		currentAttackStateTime += Time::GetDeltaTime();
		if (currentAttackStateTime >= attackStateDelay)
		{
			isAttack = false;
			currentAttackStateTime = 0.0f;
			attackStateDelay = rand() % 3 + 1.0f;
		}
		/*std::string sString = "current attack state :" + std::to_string(currentAttackStateTime);
		LOG_INFO(sString);*/
	}
}

void Eyeball::Attack()
{
	float dx = 0;
	float dy = 0;
	TranslateDirection(dx, dy);
	if ((dx != 0) && (dy != 0))
	{
		D3DXVECTOR3 location = { transform->position.x, transform->position.y - 10.0f, 0.0f };
		bullet = std::make_shared<TeleporterBullet>(dx, dy);
		bullet->CreateResources();
		SceneManager::Instantiate(bullet, location);
	}
}

void Eyeball::TranslateDirection(float& x, float& y)
{
	if (player == NULL)
	{
		player = SceneManager::GetActiveScene()->FinObjectByName("OverworldJason");
	}
	else
	{
		float playerPositionX = player->transform->position.x;
		float playerPositionY = player->transform->position.y;
		float telerporterPositionX = transform->position.x;
		float telerporterPositionY = transform->position.y;
		float vectorX = playerPositionX - telerporterPositionX;
		float vectorY = playerPositionY - telerporterPositionY;
		if ((vectorX != 0) && (vectorY != 0))
		{
			float degree = (180.0f * atan(abs(vectorX) / abs(vectorY)) / PI);
			if (vectorX > 0)
				x = sin(degree * PI / 180);
			else x = -sin(degree * PI / 180);
			if (vectorY > 0)
				y = cos(degree * PI / 180);
			else y = -cos(degree * PI / 180);
			/*std::string sString1 = "vectorX :" + std::to_string(vectorX) + "vectorY :" + std::to_string(vectorY);
			LOG_INFO(sString1);*/
			/*std::string sString2 = "x :" + std::to_string(x) + "y :" + std::to_string(y);
			LOG_INFO(sString2);*/
		}
		else
		{
			if (vectorX == 0)
			{
				x = 0;
				if (vectorY > 0)
					y = 1;
				else y = -1;
			}
			if (vectorY == 0)
			{
				y = 0;
				if (vectorX > 0)
					x = 1;
				else x = -1;
			}
		}
	}
}

void Eyeball::TranslateDirection(float& x, float& y, float posX, float posY)
{

	float vectorX = posX - transform->position.x;
	float vectorY = posY - transform->position.y;
	if ((vectorX != 0) && (vectorY != 0))
	{
		float degree = (180.0f * atan(abs(vectorX) / abs(vectorY)) / PI);
		if (vectorX > 0)
			x = sin(degree * PI / 180);
		else x = -sin(degree * PI / 180);
		if (vectorY > 0)
			y = cos(degree * PI / 180);
		else y = -cos(degree * PI / 180);
	}
	else
	{
		if (vectorX == 0)
		{
			x = 0;
			if (vectorY > 0)
				y = 1;
			else y = -1;
		}
		if (vectorY == 0)
		{
			y = 0;
			if (vectorX > 0)
				x = 1;
			else x = -1;
		}
	}
}

void Eyeball::Move()
{
	//LOG_INFO("lag");
	radiant += Time::GetDeltaTime() * PI / attackStateDelay;
	transform->position.x = centerPointX + cos(radiant) * radius;
	transform->position.y = centerPointY + sin(radiant) * radius;

	/*std::string sString = "sin X: " + std::to_string(sin(radiant)) + " cos X: " + std::to_string(cos(radiant));
	LOG_INFO(sString);*/
	//Sleep(10);
}

void Eyeball::SetTrajectory()
{
	destX = 0;
	destY = 0;
	centerPointX = 0;
	centerPointY = 0;
	if (player == NULL)
	{
		player = SceneManager::GetActiveScene()->FinObjectByName("OverworldJason");
	}
	float vectorX = player->transform->position.x - transform->position.x;
	float vectorY = player->transform->position.y - transform->position.y;
	float dX = (rand() % (int)(randomDestMaxX - randomDestMinX)) + randomDestMinX;
	float dY = (rand() % (int)(randomDestMaxY - randomDestMinY)) + randomDestMinY;
	if (vectorX < 0)
		destX = transform->position.x - dX;
	else
		destX = transform->position.x + dX;
	if (vectorY < 0)
		destY = transform->position.y - dY;
	else
		destY = transform->position.y + dY;
	centerPointX = (destX + transform->position.x) / 2;
	centerPointY = (destY + transform->position.y) / 2;
	radius = sqrt(dX * dX + dY * dY) / 2;
	radiant = asin((transform->position.y - centerPointY) / radius);
	if ((transform->position.y > centerPointY) && (transform->position.x > centerPointX))
	{
		radiant = asin((transform->position.y - centerPointY) / radius);
	}
	if ((transform->position.y > centerPointY) && (transform->position.x < centerPointX))
	{
		radiant = PI / 2 + asin((transform->position.y - centerPointY) / radius);
	}
	if ((transform->position.y < centerPointY) && (transform->position.x < centerPointX))
	{
		radiant = 3 * PI / 2 + asin((transform->position.y - centerPointY) / radius);
	}
	if ((transform->position.y < centerPointY) && (transform->position.x > centerPointX))
	{
		radiant = 4 * PI / 2 + asin((transform->position.y - centerPointY) / radius);
	}

}

void Eyeball::OnCollisionEnter(std::shared_ptr<Object2D> object)
{
	if (object->tag == Tag::Player && object->rigidbody->bodyType == Rigidbody::BodyType::Dynamic)
	{
		std::shared_ptr<OverworldJason> overworldJason = std::dynamic_pointer_cast<OverworldJason>(object);
		if (overworldJason != NULL)
		{
			overworldJason->TakeDamage(damage);
		}
	}
}

void Eyeball::OnCollisionStay(std::shared_ptr<Object2D> object)
{
	if (object->tag == Tag::Player && object->rigidbody->bodyType == Rigidbody::BodyType::Dynamic)
	{
		std::shared_ptr<OverworldJason> overworldJason = std::dynamic_pointer_cast<OverworldJason>(object);
		if (overworldJason != NULL)
		{
			overworldJason->TakeDamage(damage);
		}
	}
}

void Eyeball::SetAnimationParameter()
{
	animationController->SetBool("isAttack", isAttack);
}

void Eyeball::TakeDamage(int dmg)
{
	hitPoint -= dmg;

	if (hitPoint <= 0)
	{
		hitPoint = 0;
		Die();
	}
}

void Eyeball::Die()
{
	SceneManager::DestroyObject(shared_from_this());
}