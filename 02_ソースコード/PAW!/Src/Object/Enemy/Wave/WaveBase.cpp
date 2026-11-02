#include "../../Camera/Camera.h"
#include "WaveBase.h"

namespace
{
    VECTOR EVENT_CAMERA_POS = { 200.0f,30.0f,400.0f };
    VECTOR EVENT_CAMERA_ANGLE = 
    { -10.0f * DX_PI_F / 180.0f, -40.0f * DX_PI_F / 180.0f,0.0f };
}

WaveBase::WaveBase(void)
{
}

WaveBase::~WaveBase(void)
{
}

void WaveBase::Update(void)
{
    // ウェーブクリア済みだったら処理しない
    if (state_ == WaveState::CLEARED) return;

    // 各状態で遷移
    switch (state_)
    {
    case WaveState::EVENT:
        // イベント中
        OnEvent();

        // クリア条件を達成したら
        if (CheckEventClear())
        {
            // ネズミ行動へ
            ChangeState(WaveState::RAT);
        }
        break;

    case WaveState::RAT:
        // ネズミとの戦い
        OnRat();

        // クリア条件を達成したら
        if (CheckRatClear())
        {
            // 猫ロボット行動へ
            ChangeState(WaveState::CAT_ROBOT);
        }
        break;

    case WaveState::CAT_ROBOT:
        // 猫ロボットとの戦い
        OnCatRobot();

        // クリア条件を達成したら
        if (CheckCatRobotClear())
        {
            // ウェーブクリア
            ChangeState(WaveState::CLEARED);
            OnClear();
        }
        break;

    case WaveState::CLEARED:
        // ウェーブクリア後は特に処理なし
        break;
    }
}

void WaveBase::Draw(void)
{
    switch (state_)
    {
    case WaveBase::WaveState::EVENT:
        DrawString(
            100,
            0,
            "イベント中",
            0xff0000);
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

void WaveBase::StartInWave(void)
{
    // ウェーブ開始させる
    ChangeState(WaveState::EVENT);

    // 初期化処理
    OnStart();
}

void WaveBase::ChangeState(WaveState state)
{
    state_ = state;

    switch (state_)
    {
    case WaveBase::WaveState::EVENT:
        // カメラのモード変更
        camera_->ChangeMode(Camera::MODE::FIXED, EVENT_CAMERA_POS, EVENT_CAMERA_ANGLE);
        break;

    case WaveBase::WaveState::RAT:
        // カメラのモード変更
        camera_->ChangeMode(Camera::MODE::FOLLOW);
        break;

    case WaveBase::WaveState::CAT_ROBOT:
        break;

    case WaveBase::WaveState::CLEARED:
        break;

    default:
        break;
    }
}
