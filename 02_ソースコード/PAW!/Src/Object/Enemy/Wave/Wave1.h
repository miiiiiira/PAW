#pragma once

#include "WaveBase.h"

class Wave1 : public WaveBase
{
public:
    Wave1(void);                 // コンストラクタ
    ~Wave1(void)override;        // デストラクタ

    void Draw(void)override;    // 描画
    
protected:
    // システム起動
    void OnStart(void) override;

    // ウェーブ中
    void OnEvent(void)override;
    void OnCatRobot(void) override;

    // クリア後
    void OnClear(void) override;

    // クリア条件
    bool CheckEventClear(void)override;
    bool CheckCatRobotClear(void)override;

    // ウェーブ状態を変更する
    void ChangeInit(void)override;

private:
    // 画像ハンドル
    int defeatYouImage_ = -1;

	// アルファ値
    int alpha_ = 0;
};
