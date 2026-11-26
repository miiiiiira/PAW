#pragma once

#include "WaveBase.h"

class Wave2 : public WaveBase
{
public:
    Wave2(void);                 // コンストラクタ
    ~Wave2(void)override;        // デストラクタ

    void Draw(void)override;    // 描画

protected:
    // システム起動
    void OnStart(void) override;

    // ウェーブ中
    void OnCatRobot(void) override;

    // クリア後
    void OnEvent(void)override;
    void OnClear(void) override;

    // クリア条件
    bool CheckEventClear(void)override;
    bool CheckCatRobotClear(void)override;

    // ウェーブ状態を変更時の初期化関数
    void ChangeInit(void)override;
private:
    // 画像ハンドル
    int vsImage_ = -1;

    // アルファ値
    int alpha_ = 0;
};