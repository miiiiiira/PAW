#pragma once

#include "../../SceneBase.h"
#include <DxLib.h>


class Confirm;

class MainMenu : public SceneBase
{
public:
	enum class MENU
	{
		NONE,
		GAME_START,	// ゲーム開始ボタン
		TITLE,		// タイトルへ戻るボタン
		EXIT,		// ゲーム終了ボタン

		MAX
	};

	struct ButtonInfo
	{
		MENU type;			// 選択している種類
		int graphHandleOff;	// 選択されていない場合の画像ハンドル
		int graphHandleOn;	// 選択されている場合の画像ハンドル
		float x, y;			// 座標
		int sizeX, sizeY;	// 画像サイズ
	};

public:
	MainMenu(void);					// コンストラクタ
	~MainMenu(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:
	// 選択中の種類を変更
	void ChangeSelect(MENU menu);

	// メニュー選択時の処理
	void UpdateGameStart(void);	// ゲームスタート
	void UpdateTitle(void);		// タイトル
	void UpdateExit(void);		// ゲーム終了

	// 選択処理
	void SelectUpgrade(void);	
	void MouseSelect(void);		// マウス選択
	void PadSelect(void);		// パッド選択

private:

	std::shared_ptr<Confirm> confirm_;

	// メニューボタンの情報を格納する配列
	std::vector<ButtonInfo> menuButtons_;

	// 現在選択しているメニュー
	MENU currentMenu_;
};
