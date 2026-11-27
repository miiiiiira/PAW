#pragma once

#include "WaveBase.h"

class Wave4 : public WaveBase
{
public:
	Wave4(void);                 // コンストラクタ
	~Wave4(void)override;        // デストラクタ

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
	int goImage_ = -1;
	int everyoneAttackImage_ = -1;
	int circleRoundImage_ = -1;
	int hungryRoundImage_ = -1;

	// アルファ値
	int alpha_ = 0;
	int alpha2_ = 0;
	int alpha3_ = 0;
};