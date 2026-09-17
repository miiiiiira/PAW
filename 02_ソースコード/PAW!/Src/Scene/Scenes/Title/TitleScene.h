#pragma once
#include "../../SceneBase.h"
#include <memory>

class Confirm;

class TitleScene : public SceneBase
{

public:
	
	TitleScene(void);				// コンストラクタ
	~TitleScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// ボタンの位置
	static constexpr int BUTTON_POS_X = 512;
	static constexpr int BUTTON_POS_Y = 520;

	// ボタンのアルファ値の変化速度
	static constexpr float ALPHA_SPEED = 5.0f;

	// ボタンのアルファ値の最大値と最小値
	static constexpr float ALPHA_MAX = 255.0f;
	static constexpr float ALPHA_MIN = 0.0f;

private:

	std::shared_ptr<Confirm> confirm_;

	int backImg_ = -1;		// 背景画像
	int buttonImg_ = -1;	// PushAnyButton画像

	float alpha_;		// アルファ値
	bool isIncreasing_;	// ボタンのアルファ値が増加しているかどうか

	bool isPlaySoundSE_;	// サウンドを鳴らしたか
};
