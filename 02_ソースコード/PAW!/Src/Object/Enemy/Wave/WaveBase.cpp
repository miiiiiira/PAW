#include "../Rat/Rat.h"
#include "../../Camera/Camera.h"
#include "../../../Application.h"
#include "../../../Input/InputManager.h"

#include "WaveBase.h"
#include "../../../Audio/AudioManager.h"

namespace
{
    // スキップ画像のサイズ
    constexpr int SKIP_IMAGE_SIZE = 120;

    // スキップ画像の位置
    constexpr int SKIP_POS_X = Application::SCREEN_SIZE_X - SKIP_IMAGE_SIZE;
    constexpr int SKIP_POS_Y = Application::SCREEN_SIZE_Y - SKIP_IMAGE_SIZE;

    // ゲージの位置
    constexpr int GAUGE_POS_X = Application::SCREEN_SIZE_X - SKIP_IMAGE_SIZE / 2;
    constexpr int GAUGE_POS_Y = Application::SCREEN_SIZE_Y - SKIP_IMAGE_SIZE / 2;

    // アルファ値設定
    constexpr int ALPHA_MAX = 255;	// 最大値
    constexpr int ALPHA_STEP = 10;	// アルファ値の増減量

    // ゲージの最大値
    constexpr float GAUGE_MAX_PERCENT = 100.0f;
    // ゲージの開始位置
    constexpr double GAUGE_START_PERCENT = 0.0;
}

WaveBase::WaveBase(void)
{
	// スキップ画像のハンドルを読み込む
	skipHandle_ = LoadGraph("Data/Image/Game/skip.png");
	// スキップゲージの画像を読み込む
	skipGauge_ = LoadGraph("Data/Image/Game/skipGauge.png");
}

WaveBase::~WaveBase(void)
{
    // スキップ画像のハンドルを解放する
    DeleteGraph(skipHandle_);
    // スキップゲージの画像を解放する
	DeleteGraph(skipGauge_);
}

void WaveBase::Update(void)
{
    // ウェーブクリア済みだったら処理しない
    if (state_ == WAVE_STATE::CLEARED) return;

    // 各状態で遷移
    switch (state_)
    {
    case WAVE_STATE::EVENT:
         // 一定までスペースキーを押したらイベントシーンをスキップする
         if (InputManager::GetInstance()->SkipButton())
         {
             skipTimer_++;

             alpha_ += ALPHA_STEP;
             if (alpha_ >= ALPHA_MAX)
             {
                 alpha_ = ALPHA_MAX;
             }
         }
         else
         {
             skipTimer_ = 0;

             alpha_ -= ALPHA_STEP;
             if (alpha_ <= 0)
             {
                 alpha_ = 0;
             }
         }

        // イベント中
        OnEvent();

        // クリア条件を達成したら
        if (CheckEventClear())
        {
            // カウント初期化
            ratToCatRobotCount_ = 0;
            // ネズミ行動へ
            ChangeState(WAVE_STATE::RAT);
        }
        break;

    case WAVE_STATE::RAT:
        // ネズミとの戦い
        OnRat();

        // クリア条件を達成したら
        if (CheckRatClear())
        {
            // 猫ロボット行動へ
            ChangeState(WAVE_STATE::CAT_ROBOT);
        }
        break;

    case WAVE_STATE::CAT_ROBOT:
        // 猫ロボットとの戦い
        OnCatRobot();

        // クリア条件を達成したら
        if (CheckCatRobotClear())
        {
            // ウェーブクリア
            ChangeState(WAVE_STATE::CLEARED);
            OnClear();
        }
        break;

    case WAVE_STATE::CLEARED:
        // ウェーブクリア後は特に処理なし
        break;
    }
}

void WaveBase::Draw(void)
{
    // アルファ値を設定
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);

    float rate = (static_cast<float>(skipTimer_) / MAX_SKIP_TIME) * GAUGE_MAX_PERCENT;
    DrawCircleGauge(GAUGE_POS_X, GAUGE_POS_Y, rate, skipGauge_, GAUGE_START_PERCENT);
    DrawGraph(SKIP_POS_X, SKIP_POS_Y, skipHandle_, true);

    // アルファ値を元に戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void WaveBase::StartInWave(void)
{
    // ウェーブ開始させる
    ChangeState(WAVE_STATE::EVENT);

    // 初期化処理
    OnStart();
}

void WaveBase::OnEvent(void)
{
    // BGM再生
    AudioManager::GetInstance()->PlayBGM(SoundID::BGM_RAT_EVENT);
}

void WaveBase::OnRat(void)
{
    // タイマーとアルファ値の初期化
    skipTimer_ = 0;
    alpha_ = 0;
}

bool WaveBase::CheckRatClear(void)
{
    // 全ネズミ死亡しているか
    for (const auto& rat : rats_)
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
    if (ratToCatRobotCount_ < RAT_TO_CATROBOT_COUNT) return false;
    return true;
}

void WaveBase::ChangeState(WAVE_STATE state)
{
    state_ = state;

    switch (state_)
    {
    case WaveBase::WAVE_STATE::EVENT:
        // カメラのモード変更
        camera_->ChangeMode(Camera::MODE::FIXED, EVENT_CAMERA_POS, EVENT_CAMERA_ANGLE);
        break;

    case WaveBase::WAVE_STATE::RAT:
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