#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "Wave1.h"

namespace
{
	constexpr VECTOR RAT_SPAWN_POS = { 0.0f, 0.0f, 1500.0f };
}

Wave1::Wave1(void)
{
}

Wave1::~Wave1(void)
{
}

void Wave1::OnStart(void)
{
	rats_[0]->ResetRat(RAT_SPAWN_POS);
}

void Wave1::OnEvent(void)
{
}

void Wave1::OnRat(void)
{
}

void Wave1::OnCatRobot(void)
{
}

void Wave1::OnClear(void)
{
}

bool Wave1::CheckEventClear(void)
{
	// イベント移動が終了したら
	if (rats_[0]->GetState() != RAT_STATE_EVENT_MOVE &&
		rats_[0]->GetState() != RAT_STATE_EVENT_IDLE)
	{
		return true;
	}

	return false;
}

bool Wave1::CheckRatClear(void)
{
	bool isAllDead = true;

	// 全ネズミ死亡しているか
	for (const auto& rat: rats_)
	{
		// 完全終了までいっていなかったら
		if (rat->GetState() != RAT_STATE_END)
		{
			// フラグを折る
			isAllDead = false;
		}
	}

	return isAllDead;
}

bool Wave1::CheckCatRobotClear(void)
{
	if (catRobot_->GetNowHP() <= 0)
	{
		return true;
	}

	return false;
}
