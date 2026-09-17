#pragma once

#include "../../SceneBase.h"
#include "../../../Application.h"
#include <vector>

class GameOver : public SceneBase
{
public:

	// 選択ボタンの種類
	enum TYPE
	{
		RETRY,			// リトライ
		RETURN_TITLE,	// タイトルへ戻る
		NONE,
	};

	struct ButtonInfo
	{
		TYPE type;
		int graphHandle;
		int x, y;
		int sizeX, sizeY;
	};

	GameOver(void);					// コンストラクタ
	~GameOver(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	void SelectUpdate(void);	// 選択処理
	
	void MouseSelect(void);		// マウス選択

	void PadSelect(void);		// パッド選択

private:

	// RETRY画像サイズ
	static constexpr int RETRY_SIZE_X = 152;
	static constexpr int RETRY_SIZE_Y = 32;

	// RETRY
	static constexpr int RETRY_POS_X = Application::SCREEN_SIZE_X / 2 - RETRY_SIZE_X / 2;
	static constexpr int RETRY_POS_Y = 485;

	// RETURN_TITLE画像サイズ
	static constexpr int RETURN_TITLE_SIZE_X = 152;
	static constexpr int RETURN_TITLE_SIZE_Y = 32;

	// RETURN_TITLE
	static constexpr int RETURN_TITLE_POS_X = Application::SCREEN_SIZE_X / 2 - RETURN_TITLE_SIZE_X / 2;
	static constexpr int RETURN_TITLE_POS_Y = 550;

	static constexpr int FRAME_OFFSET = 10;	// フレームのオフセット


private:

	// 画像ハンドル
	int handle_ = -1;		// ゲームオーバー画像

	std::vector<ButtonInfo> buttons_;		// ボタンの情報を格納する配列
	
	TYPE currentType_;	// 現在選択しているメニュー

};

