#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "../../../Audio/AudioManager.h"
#include "../../Component/Transform/Transform.h"
#include "../../Camera/Camera.h"
#include "../../../System/Fader/Fader.h"

#include "Wave1.h"

namespace
{
	// イベントシーン時のネズミの開始座標
	constexpr VECTOR RAT_SPAWN_POS = { 0.0f, 0.0f, 1500.0f };

	// イベントシーン時のアニメーション変更時間
	constexpr int EVENT_CHANGE_ANIM_TIME = 50;

	// アルファ値設定
	constexpr int ALPHA_STEP = 10;	// アルファ値の増減量
}

Wave1::Wave1(void)
{
	// 画像読み込み
	defeatYouImage_ = LoadGraph("Data/Image/Game/defeatYou.png");
}

Wave1::~Wave1(void)
{
	// 画像解放
	DeleteGraph(defeatYouImage_);
}

void Wave1::Draw(void)
{
	if(state_ == WaveBase::WAVE_STATE::EVENT)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);
		// 倒してやる画像
		DrawBillboard3D(
			{ rats_[0]->GetTransform()->pos_.x + 80.0f ,
			rats_[0]->GetTransform()->pos_.y + 120.0f ,
			rats_[0]->GetTransform()->pos_.z - 10.0f },
			0.7f, 0.7f,
			80.0f,
			-0.1f,
			defeatYouImage_,
			true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 基底クラスの描画処理
	WaveBase::Draw();
}

void Wave1::OnStart(void)
{
	rats_[0]->ResetRat(RAT_SPAWN_POS, RAT_ANIMATION::ORDER, EVENT_CHANGE_ANIM_TIME, false);
}

void Wave1::OnEvent(void)
{
	// 基底クラスのイベント処理
	WaveBase::OnEvent();

	if (alpha_ > 0)
	{
		alpha_ += ALPHA_STEP;
		if (alpha_ > ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
		}
	}
	else
	{
		// ネズミがイベント待機状態になったらアルファ値を増やす
		if (rats_[0]->GetState() == RAT_STATE_EVENT_IDLE)
		{
			alpha_++;
		}
	}
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
	if (skipTimer_ >= MAX_SKIP_TIME ||
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

void Wave1::ChangeInit(void)
{
	switch (state_)
	{
	case WaveBase::WAVE_STATE::EVENT:
		// BGM再生
		AudioManager::GetInstance()->PlayBGM(SoundID::BGM_RAT_EVENT);
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
		Fader::GetInstance()->SetFade(Fader::STATE::FADE_OUT, Fader::TYPE::CROSS, true);
		break;

	default:
		break;
	}
}
