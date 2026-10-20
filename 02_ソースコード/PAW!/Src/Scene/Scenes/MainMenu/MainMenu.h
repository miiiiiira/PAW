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
		MENU type;
		int graphHandleOff;
		int graphHandleOn;
		float x, y;
		int sizeX, sizeY;
	};

	MainMenu(void);				// コンストラクタ
	~MainMenu(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	void ChangeSelect(MENU menu);

	// メニュー選択時の処理
	void UpdateGameStart(void);
	void UpdateTitle(void);
	void UpdateExit(void);

	void SelectUpgrade(void);	// 選択処理

	void MouseSelect(void);		// マウス選択

	void PadSelect(void);		// パッド選択

private:

	std::shared_ptr<Confirm> confirm_;

	// メニューボタンの情報を格納する配列
	std::vector<ButtonInfo> menuButtons_;

	// 現在選択しているメニュー
	MENU currentMenu_;
};
