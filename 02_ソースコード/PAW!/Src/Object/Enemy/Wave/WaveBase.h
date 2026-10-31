#pragma once

#include <DxLib.h>

class WaveBase
{
public:
    // ウェーブの状態
    enum class WaveState
    {
        EVENT,     // イベント
        RAT,       // ネズミ
        CAT_ROBOT, // 猫ロボット
        CLEARED    // クリア済み
    };

public:
    // コンストラクタで設定
    WaveBase(void);
    virtual ~WaveBase(void);

    virtual void Update(void);  // 更新
    virtual void Draw(void);    // 描画

public:
    // ウェーブの状態を渡す
    WaveState GetState(void)const { return state_; }

    // ウェーブ開始させる
    void StartInWave(void);

protected:
    // 固定フレーム
    static constexpr int ONE_SECOND = 60;

protected:
    // ウェーブ状態
    WaveState state_ = WaveState::EVENT;

protected:
    // システム起動
    virtual void OnStart(void) {};   

    // 準備期間中
    virtual void OnPrepare(void) {};    

    // ウェーブ中
    virtual void OnEvent(void) {};
    virtual void OnRat(void) {};   
    virtual void OnCatRobot(void) {};   

    // クリア後
    virtual void OnClear(void) {};       

    // クリア条件
    virtual bool CheckEventClear(void) = 0;
    virtual bool CheckRatClear(void) = 0;
    virtual bool CheckCatRobotClear(void) = 0;

};