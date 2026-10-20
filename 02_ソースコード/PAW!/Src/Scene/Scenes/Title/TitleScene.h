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

	std::shared_ptr<Confirm> confirm_;

	int backImg_ = -1;		// 背景画像
	int buttonImg_ = -1;	// PushAnyButton画像

	float alpha_;		// アルファ値
	bool isIncreasing_;	// ボタンのアルファ値が増加しているかどうか

	bool isPlaySoundSE_;	// サウンドを鳴らしたか
};
