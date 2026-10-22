#pragma once
#include "../../SceneBase.h"
#include <memory>

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
	// 画像ハンドル
	int backImg_ = -1;		// 背景
	int buttonImg_ = -1;	// PushAnyButton

	// アルファ値系
	float alpha_ = 0.0f;		// 実際の値
	bool isIncreasing_ = false;	// ボタンのアルファ値が増加しているかどうか

	// サウンドを鳴らしたか
	bool isPlaySoundSE_ = false;	

private:
	// メニュー画面
	bool ChangeMainMenu(void);

	// 確認画面
	bool ChangeConfirmScene(void);

	// 条件に合わせてアルファ値を加減算する
	void UpdateAlpha(void);
};
