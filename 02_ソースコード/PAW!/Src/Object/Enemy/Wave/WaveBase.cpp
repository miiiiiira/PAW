#include "WaveBase.h"

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
            state_ = WaveState::RAT;
        }
        break;

    case WaveState::RAT:
        // ネズミとの戦い
        OnRat();

        // クリア条件を達成したら
        if (CheckRatClear())
        {
            // 猫ロボット行動へ
            state_ = WaveState::CAT_ROBOT;
        }
        break;

    case WaveState::CAT_ROBOT:
        // 猫ロボットとの戦い
        OnCatRobot();

        // クリア条件を達成したら
        if (CheckCatRobotClear())
        {
            // ウェーブクリア
            state_ = WaveState::CLEARED;
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
    state_ = WaveState::EVENT;

    // 初期化処理
    OnStart();
}
