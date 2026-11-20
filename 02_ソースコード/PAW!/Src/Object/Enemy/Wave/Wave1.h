#pragma once

#include "WaveBase.h"

class Wave1 : public WaveBase
{
public:
    Wave1(void);                 // コンストラクタ
    ~Wave1(void)override;        // デストラクタ

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

    // ウェーブ状態を変更する
    void ChangeState(WAVE_STATE state)override;

private:
    int taositeyaruImage_ = -1;
};
