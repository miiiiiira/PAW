#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "Wave1.h"

Wave1::Wave1(void)
{
}

Wave1::~Wave1(void)
{
}

void Wave1::OnStart(void)
{
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
	return true;
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
