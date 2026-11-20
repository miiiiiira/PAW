#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "../../../Audio/AudioManager.h"
#include "../../Component/Transform/Transform.h"
#include "../../Camera/Camera.h"

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
	case WaveBase::WAVE_STATE::EVENT:
		if (rats_[0]->GetState() == RAT_STATE_EVENT_IDLE)
		{
			// 倒してやる画像
			DrawBillboard3D(
				{ rats_[0]->GetTransform()->pos_.x + 60.0f ,
				rats_[0]->GetTransform()->pos_.y + 120.0f , 
				rats_[0]->GetTransform()->pos_.z - 20.0f },
				0.7f, 0.7f,
				64.0f,
				-0.1f, 
				taositeyaruImage_,
				true);
		}
		break;

	case WaveBase::WAVE_STATE::RAT:
		break;

	case WaveBase::WAVE_STATE::CAT_ROBOT:
		break;

	case WaveBase::WAVE_STATE::CLEARED:
		break;

	default:
		break;
	}

	// 基底クラスの描画処理
	WaveBase::Draw();
}

void Wave1::OnStart(void)
{
	rats_[0]->ResetRat(RAT_SPAWN_POS);
}

void Wave1::OnCatRobot(void)
{
}

void Wave1::OnClear(void)
{
}

bool Wave1::CheckEventClear(void)
{
	// スキップ処理を一定以上押されるか、イベント移動が終了したら
	if (skipTimer_ >= MAX_SKIP_TIME||
		rats_[0]->IsEndEventMove())
	{
		// BGMを鳴らす
		AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME);
		return true;
	}

	return false;
}

bool Wave1::CheckCatRobotClear(void)
{
	if (catRobot_->GetNowHP() <= CatRobot::DEFAULT_HP - 5)
	{
		return true;
	}

	return false;
}

void Wave1::ChangeState(WAVE_STATE state)
{
	state_ = state;

	switch (state_)
	{
	case WaveBase::WAVE_STATE::EVENT:
		// カメラのモード変更
		camera_->ChangeMode(Camera::MODE::FIXED, EVENT_CAMERA_POS, EVENT_CAMERA_ANGLE);
		break;

	case WaveBase::WAVE_STATE::RAT:
		rats_[0]->SetEventEndPos(RAT_SPAWN_POS);

		// カメラのモード変更
		camera_->ChangeMode(Camera::MODE::FOLLOW);
		break;

	case WaveBase::WAVE_STATE::CAT_ROBOT:
		break;

	case WaveBase::WAVE_STATE::CLEARED:
		break;

	default:
		break;
	}
}
