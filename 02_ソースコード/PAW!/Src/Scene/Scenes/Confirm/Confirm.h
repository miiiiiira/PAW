#pragma once

#include "../../SceneBase.h"
#include <DxLib.h>
#include "../../../Application.h"

class Confirm : public SceneBase
{
public:

	enum class TYPE
	{
		NONE,
		QUIT,
		MAIN_MENU,
	};

	enum class SELECT
	{
		NONE,
		YES,
		NO,
	};

	struct ButtonInfo
	{
		SELECT type;
		int graphHandle;
		int x, y;
		int sizeX, sizeY;
	};

public:

	Confirm(void);					// コンストラクタ
	~Confirm(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

	void ChangeType(TYPE type);

private:

	// 確認ウィンドウの位置とサイズ
	static constexpr int CONFIRM_SIZE_X = 800;
	static constexpr int CONFIRM_SIZE_Y = 500;
	static constexpr int CONFIRM_POS_X = (Application::SCREEN_SIZE_X - CONFIRM_SIZE_X) / 2;
	static constexpr int CONFIRM_POS_Y = (Application::SCREEN_SIZE_Y - CONFIRM_SIZE_Y) / 2;

	// 画像サイズ（YES, NO）
	static constexpr int IMAGE_SIZE_X = 150;
	static constexpr int IMAGE_SIZE_Y = 70;

	// YES
	static constexpr int YES_POS_X = (Application::SCREEN_SIZE_X - IMAGE_SIZE_X) / 2 - 150;
	static constexpr int YES_POS_Y = (Application::SCREEN_SIZE_Y - IMAGE_SIZE_Y) / 2 + 150;

	// NO
	static constexpr int NO_POS_X = (Application::SCREEN_SIZE_X - IMAGE_SIZE_X) / 2 + 150;
	static constexpr int NO_POS_Y = YES_POS_Y;

	// フレームのオフセット
	static constexpr int FRAME_OFFSET = 10;

private:

	void ChangeSelect(SELECT select);

	// メニュー選択時の処理
	void UpdateYes(void);
	void UpdateNo(void);

	void SelectUpgrade(void);	// 選択処理
	
	void MouseSelect(void);		// マウス選択
	
	void PadSelect(void);		// パッド選択

private:
	std::vector<ButtonInfo> selectButtons_;	// 選択肢のボタン

	SELECT currentSelect_;				// 現在の選択肢

	TYPE confirmType_;					// どの確認画面化

	int confirmImg_;					// 画像ハンドル
};

