#pragma once

#include "../../SceneBase.h"
#include <DxLib.h>

class Confirm;

class Pause : public SceneBase
{
public:
	enum class PAUSE
	{
		NONE,
		CONTINUE,
		TITLE,
		EXIT,

		MAX
	};

	struct ButtonInfo
	{
		PAUSE type;			// 選択している種類
		int graphHandleOff;	// 選択されていない場合の画像ハンドル
		int graphHandleOn;	// 選択されている場合の画像ハンドル
		float x, y;			// 座標
		int sizeX, sizeY;	// 画像サイズ
	};

public:
	Pause(void);				// コンストラクタ
	~Pause(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:
	// 背景画像ハンドル
	int backImage_ = -1;

	// ポーズボタンの情報を格納する配列
	std::vector<ButtonInfo>  pauseButtons_;

	// 現在選択されているポーズ
	PAUSE currentPause_;

private:
	// 選択中の種類を変更
	void ChangeSelect(PAUSE pause);

	// 選択時の処理
	void UpdateContinue(void);	// コンテニュー
	void UpdateTitle(void);		// タイトル
	void UpdateExit(void);		// ゲーム終了

	// 選択処理
	void SelectUpgrade(void);	
	void MouseSelect(void);		// マウス選択
	void PadSelect(void);		// パッド選択
	
};