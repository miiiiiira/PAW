#pragma once

#include <DxLib.h>
#include <vector>

class Rat;
class CatRobot;
class Camera;

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

    // ネズミと猫ロボットのポインタをセット
    void SetRatCatRobotPoint(std::vector<Rat*>rats, CatRobot* catRobot, Camera* camera)
    {
        rats_ = rats, catRobot_ = catRobot, camera_ = camera;
    }

protected:
    // 固定フレーム
    static constexpr int ONE_SECOND = 60;

    // ネズミから猫ロボット時のカウント
    static constexpr int RAT_TO_CATROBOT_COUNT = 120;

	// イベントシーン時のカメラ座標
    static constexpr VECTOR EVENT_CAMERA_POS = { 200.0f,30.0f,400.0f };
    static constexpr VECTOR EVENT_CAMERA_ANGLE =
    { -10.0f * DX_PI_F / 180.0f, -40.0f * DX_PI_F / 180.0f,0.0f };

    // スキップ時間の最大値
    static constexpr float MAX_SKIP_TIME = 120.0f;

protected:
    std::vector<Rat*>rats_;
    CatRobot* catRobot_;
    Camera* camera_;

protected:
    // ウェーブ状態
    WaveState state_ = WaveState::EVENT;

    // ネズミから猫ロボット時のカウント
    int ratToCatRobotCount_ = 0;

    // スキップタイマー
    int skipTimer_ = 0;

    // スキップ画像のハンドルとゲージのハンドル
    int skipHandle_ = 0;
    int skipGauge_ = 0;

    // アルファ値
    int alpha_ = 0;

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
    virtual bool CheckRatClear(void);
    virtual bool CheckCatRobotClear(void) = 0;

	// ウェーブ状態を変更する
    virtual void ChangeState(WaveState state);
};