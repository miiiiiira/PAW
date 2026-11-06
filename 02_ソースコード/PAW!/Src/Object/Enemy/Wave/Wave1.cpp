#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "../../../Audio/AudioManager.h"
#include "../../Component/Transform/Transform.h"

#include "Wave1.h"

namespace
{
	constexpr VECTOR RAT_SPAWN_POS = { 0.0f, 0.0f, 1500.0f };
}

Wave1::Wave1(void)
{
	// 画像読み込み
	taositeyaruImage_ = LoadGraph("Data/Image/Game/taositeyaru.png");
}

Wave1::~Wave1(void)
{
	// 画像解放
	DeleteGraph(taositeyaruImage_);
}

void Wave1::Draw(void)
{
	switch (state_)
	{
	case WaveBase::WaveState::EVENT:
		// 倒してやる画像
		DrawBillboard3D({ rats_[0]->GetTransform()->pos_.x + 50.0f , rats_[0]->GetTransform()->pos_.y + 100.0f , rats_[0]->GetTransform()->pos_.z - 20.0f }, 0.5f, 0.5f, 64.0f, -0.1, taositeyaruImage_, true);
		break;

	case WaveBase::WaveState::RAT:
		break;

	case WaveBase::WaveState::CAT_ROBOT:
		break;

	case WaveBase::WaveState::CLEARED:
		break;

	default:
		break;
	}

}

void Wave1::OnStart(void)
{
	// BGM再生
	rats_[0]->ResetRat(RAT_SPAWN_POS);
}

void Wave1::OnEvent(void)
{
	// BGM再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_RAT_EVENT);
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
		// BGMを鳴らす
		AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME);
		return true;
	}

	return false;
}

bool Wave1::CheckRatClear(void)
{
	// 全ネズミ死亡しているか
	for (const auto& rat: rats_)
	{
		// 完全終了までいっていなかったら
		if (rat->GetState() != RAT_STATE_END)
		{
			// フラグを折る
			return false;
		}
	}

	// カウントを進める。
	ratToCatRobotCount_++;

	// カウントが指定値に達していなかったら処理を行わない
	if (ratToCatRobotCount_ < RAT_TO_CATROBOT_COUNT)return false;

	return true;
}

bool Wave1::CheckCatRobotClear(void)
{
	if (catRobot_->GetNowHP() <= 0)
	{
		return true;
	}

	return false;
}
