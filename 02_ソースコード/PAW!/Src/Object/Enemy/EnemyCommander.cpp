#include "../ObjectManager.h"
#include "CatRobot/CatRobot.h"
#include "Rat/Rat.h"

#include "EnemyCommander.h"

EnemyCommander::EnemyCommander(void)
{
}

void EnemyCommander::Init(std::vector<Rat*> rats, CatRobot* catRobot)
{
	rats_ = rats;

	catRobot_ = catRobot;

	float i = -1.0f;
	// ネズミの攻撃処理
	for (const auto& rat : rats_)
	{
		Transform* ratTransform = rat->GetTransform();
		
		ratTransform->pos_ = { i * 50.0f,0.0f,150.0f };
		i++;
	}
}

void EnemyCommander::Update(void)
{
	// ネズミ司令処理
	RatControl();
}

void EnemyCommander::Release(void)
{
	rats_.clear();
}

void EnemyCommander::RatControl(void)
{
	int i = 0;

	// ネズミの攻撃処理
	for (const auto& rat : rats_)
	{
		// 攻撃状態のネズミを2匹までとする
		if (i >= 2)break;

		// 攻撃状態だったら
		if (rat->GetState() == RAT_STATE_ATTACK)
		{
			// カウントして次へ
			i++;
			continue;
		}

		// 待機状態だったら
		if (rat->IsCanAttack())
		{
			// 攻撃処理へ
			rat->ChangeState(RAT_STATE_ATTACK);
			i++;
		}
	}

	// 猫ロボットが待機状態だったら
	if (catRobot_->GetState() == CAT_ROBOT_STATE_IDLE)
	{
		// 攻撃状態へ
		catRobot_->ChangeState(CAT_ROBOT_STATE_ATTACK);
	}
}
