#pragma once

#include "WaveBase.h"

class Wave2 : public WaveBase
{
public:
    Wave2(void);                 // コンストラクタ
    ~Wave2(void)override;        // デストラクタ

    void Draw(void)override;    // 描画

protected:
    // 準備時間
    static constexpr int PREPARE_TIME = ONE_SECOND * 20;

protected:
    // システム起動
    void OnStart(void) override;

    // ウェーブ中
    void OnCatRobot(void) override;

    // クリア後
    void OnClear(void) override;

    // クリア条件
    bool CheckEventClear(void)override;
    bool CheckCatRobotClear(void)override;

    // ウェーブ状態を変更時の初期化関数
    void ChangeInit(void)override;
};