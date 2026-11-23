#include "../../Player/Player.h"
#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "../../../Audio/AudioManager.h"
#include "../../Component/Transform/Transform.h"
#include "../../Camera/Camera.h"
#include "../../../System/Fader/Fader.h"

#include "Wave4.h"

namespace
{
	constexpr VECTOR RAT_SPAWN_POS = { -60.0f, 0.0f, 1600.0f };
	constexpr VECTOR RAT2_SPAWN_POS = { 60.0f, 0.0f, 1600.0f };
	constexpr VECTOR RAT3_SPAWN_POS = { -160.0f, 0.0f, 1500.0f };
	constexpr VECTOR RAT4_SPAWN_POS = { 160.0f, 0.0f, 1500.0f };
}

Wave4::Wave4(void)
{
}

Wave4::~Wave4(void)
{
}

void Wave4::Draw(void)
{
	// 基底クラスの描画処理
	WaveBase::Draw();
}

void Wave4::OnStart(void)
{
	player_->ResetPlayer();
	rats_[0]->ResetRat(RAT_SPAWN_POS);
	rats_[1]->ResetRat(RAT2_SPAWN_POS);
	rats_[2]->ResetRat(RAT3_SPAWN_POS);
	rats_[3]->ResetRat(RAT4_SPAWN_POS);
}

void Wave4::OnCatRobot(void)
{
}

void Wave4::OnClear(void)
{
}

bool Wave4::CheckEventClear(void)
{
	// 全ネズミがイベント移動中かイベント待機中かを確認
	bool allClear = true;
	for (int i = 0; i < 4; i++)
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

bool Wave4::CheckCatRobotClear(void)
{
	if (catRobot_->GetNowHP() <= 0)
	{
		return true;
	}

	return false;
}

void Wave4::ChangeInit(void)
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
		rats_[2]->SetEventEndPos(RAT3_SPAWN_POS);
		rats_[3]->SetEventEndPos(RAT4_SPAWN_POS);

		// カメラのモード変更
		camera_->ChangeMode(Camera::MODE::FOLLOW);
		break;

	case WaveBase::WAVE_STATE::CAT_ROBOT:
		break;

	case WaveBase::WAVE_STATE::CLEARED:
		Fader::GetInstance()->SetFade(Fader::STATE::FADE_OUT, Fader::TYPE::CROSS, true);
		break;

	default:
		break;
	}
}
