#include "../../Player/Player.h"
#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "../../../Audio/AudioManager.h"
#include "../../Component/Transform/Transform.h"
#include "../../Camera/Camera.h"
#include "../../../System/Fader/Fader.h"

#include "Wave2.h"

namespace
{
	constexpr VECTOR RAT_SPAWN_POS = { -100.0f, 0.0f, 1500.0f };
	constexpr VECTOR RAT2_SPAWN_POS = { 100.0f, 0.0f, 1500.0f };
}

Wave2::Wave2(void)
{
}

Wave2::~Wave2(void)
{
}

void Wave2::Draw(void)
{
	// 基底クラスの描画処理
	WaveBase::Draw();
}

void Wave2::OnStart(void)
{
	player_->ResetPlayer();
	rats_[0]->ResetRat(RAT_SPAWN_POS);
	rats_[1]->ResetRat(RAT2_SPAWN_POS);
}

void Wave2::OnCatRobot(void)
{
}

void Wave2::OnClear(void)
{
}

bool Wave2::CheckEventClear(void)
{
	// 全ネズミがイベント移動中かイベント待機中かを確認
	bool allClear = true;
	for (int i = 0; i < 2; i++)
	{
		if (!rats_[i]->IsEndEventMove())
		{
			allClear = false;
		}
	}

	// スキップ処理を一定以上押されるか、イベント移動が終了したら
	if (skipTimer_ >= MAX_SKIP_TIME ||
		allClear)
	{
		// BGMを鳴らす
		AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME);
		return true;
	}

	return false;
}

bool Wave2::CheckCatRobotClear(void)
{
	if (catRobot_->GetNowHP() <= CatRobot::DEFAULT_HP - 10)
	{
		return true;
	}

	return false;
}

void Wave2::ChangeInit(void)
{
	switch (state_)
	{
	case WaveBase::WAVE_STATE::EVENT:
		// カメラのモード変更
		camera_->ChangeMode(Camera::MODE::FIXED, EVENT_CAMERA_POS, EVENT_CAMERA_ANGLE);
		break;

	case WaveBase::WAVE_STATE::RAT:
		rats_[0]->SetEventEndPos(RAT_SPAWN_POS);
		rats_[1]->SetEventEndPos(RAT2_SPAWN_POS);

		// カメラのモード変更
		camera_->ChangeMode(Camera::MODE::FOLLOW);
		break;

	case WaveBase::WAVE_STATE::CAT_ROBOT:
		break;

	case WaveBase::WAVE_STATE::CLEARED:
		//Fader::GetInstance()->SetFade(Fader::STATE::FADE_OUT);
		break;

	default:
		break;
	}
}
