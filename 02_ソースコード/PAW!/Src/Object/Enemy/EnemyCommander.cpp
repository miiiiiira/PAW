#include "../ObjectManager.h"
#include "CatRobot/CatRobot.h"
#include "Rat/Rat.h"
#include "Wave/WaveManager.h"

#include "EnemyCommander.h"

namespace
{
	// 攻撃させるネズミの最大数
	constexpr int RAT_ATTACK_NUM = 2;
	// 時間を空けて攻撃させるため用のカウント
	constexpr int ATTACK_START_COUNT = 360;
}

EnemyCommander::EnemyCommander(void)
{
}

void EnemyCommander::Init(std::vector<Rat*> rats, CatRobot* catRobot)
{
	rats_ = rats;

	catRobot_ = catRobot;

	// 初期化
	ratAttackQueue_.clear();
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

void EnemyCommander::EnemyControl(void)
{
	switch (WaveManager::GetInstance().GetCurrentWave()->GetState())
	{
	case WaveBase::WaveState::RAT:
		RatControl();
		break;
	case WaveBase::WaveState::CAT_ROBOT:
		CatRobotControl();
		break;

	case WaveBase::WaveState::EVENT:
	case WaveBase::WaveState::CLEARED:
	default:
		break;
	}
}

void EnemyCommander::RatControl(void)
{
	// まだカウントが残っていたら
	if (ratAttackStartCount_ > 0)
	{
		// カウントを減らす
		ratAttackStartCount_--;
		return;
	}

	// 攻撃指示
	RatAttackOrder();

	// 中身があれば処理を行わない
	if (!ratAttackQueue_.empty())return;

	// 誰を攻撃させるか決める
	SetRatAttackIndex();
}

void EnemyCommander::RatAttackOrder(void)
{
	// 中身がなければ処理を行わない
	if (ratAttackQueue_.empty())return;

	Rat* rat = ratAttackQueue_.front();

	// 死亡していたら
	if (rat->GetState() == RAT_STATE_DEAD ||
		rat->GetState() == RAT_STATE_END)
	{
		// 要素を削除
		ratAttackQueue_.erase(ratAttackQueue_.begin());
		SetRatAttackIndex();
		return;
	}

	// 待機中であれば
	if (rat->GetState() == RAT_STATE_IDLE)
	{
		// 選ばれたネズミを攻撃処理へ
		rat->ChangeState(RAT_STATE_ATTACK);
		return;
	}

	// 攻撃終了していたら
	if (rat->IsAttackEnd())
	{
		// 要素を削除
		ratAttackQueue_.erase(ratAttackQueue_.begin());

		// 中身がなければ
		if (ratAttackQueue_.empty())
		{
			// 攻撃指示が終えてるため時間を空けさせる
			ratAttackStartCount_ = ATTACK_START_COUNT;
		}
	}
}

void EnemyCommander::SetRatAttackIndex(void)
{
	// ネズミの攻撃処理
	for (const auto& rat : rats_)
	{
		// 攻撃状態のネズミを2匹までとする
		if (ratAttackQueue_.size() >= RAT_ATTACK_NUM)break;

		// 待機状態だったら
		if (rat->IsCanAttack())
		{
			// 攻撃指定させるネズミの添え字を保存
			ratAttackQueue_.emplace_back(rat);
		}
	}
}

void EnemyCommander::CatRobotControl(void)
{
	// 猫ロボットが待機状態だったら
	if (catRobot_->GetState() == CAT_ROBOT_STATE_IDLE)
	{
		// 攻撃状態へ
		catRobot_->ChangeState(CAT_ROBOT_STATE_ATTACK);
	}
}
