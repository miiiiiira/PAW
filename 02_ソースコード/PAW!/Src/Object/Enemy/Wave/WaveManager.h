#pragma once

#include "WaveBase.h"

#include <memory>
#include <vector>

class Rat;
class CatRobot;

class WaveManager
{
public:
    // シングルトン（生成・取得・削除）
    static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new WaveManager(); } };
    static WaveManager& GetInstance(void) { return *instance_; };
    static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:
    // 静的インスタンス
    static WaveManager* instance_;

    WaveManager(void);  // コンストラクタ
    ~WaveManager(void); // デストラクタ

    // コピー・ムーブ操作を禁止
    WaveManager(const WaveManager&) = delete;
    WaveManager& operator=(const WaveManager&) = delete;
    WaveManager(WaveManager&&) = delete;
    WaveManager& operator=(WaveManager&&) = delete;

public:
    void Update(void);  // 更新
    void Draw(void);    // 描画

public:
    // ウェーブの追加
    void AddWave(
        std::unique_ptr<WaveBase> wave,
        std::vector<Rat*> rats,
        CatRobot* catRobot);

    // 現在のウェーブを取得
    WaveBase* GetCurrentWave(void);
    // 次のウェーブを取得
    WaveBase* GetNextWave(void);

    // ウェーブを全てクリアしているか
    bool AllCleared(void)const;

    // 1ウェーブがクリアしたか
    bool GetWaveIsClear(void)const { return waveIsClear_; }
    void EndWaveIsClear(void) { waveIsClear_ = false; }

private:
    // 各ウェーブを格納
    std::vector<std::unique_ptr<WaveBase>> waves_;
    // 今現在のウェーブ
    int currentWaveIndex_ = 0;

    // 1ウェーブがクリアしたか
    bool waveIsClear_ = false;

};

