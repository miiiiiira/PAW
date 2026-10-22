#pragma once

#include "../../SceneBase.h"
#include <DxLib.h>
#include "../../../Application.h"

class Confirm : public SceneBase
{
public:
	// 選択中の種類
	enum class SELECT
	{
		NONE,
		YES,
		NO,

		MAX
	};

	// 猫の種類
	enum class CAT_TYPE
	{
		OPEN_EYES,		// 目を開けている
		CLOSING_EYES,	// 目を閉じている途中
		CLOSE_EYE,		// 目を閉じている

		MAX
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

private:
	// 画像ハンドル
	int confirmImg_ = -1;							// 確認画面
	int catImg_[static_cast<int>(CAT_TYPE::MAX)];	// 猫

	// 選択肢のボタン
	std::vector<ButtonInfo> selectButtons_;

	// 現在の選択肢
	SELECT currentSelect_ = SELECT::NONE;

	// 猫
	int changeCatCount = 0;					// 変更までのカウント
	bool isOpenEye = true;					// 目を開けているか
	CAT_TYPE currentCat_ = CAT_TYPE::MAX;	// 種類

private:
	// 現在の選択肢設定
	void ChangeSelect(SELECT select) { currentSelect_ = select; }

	// 猫の種類設定
	void ChangeCat(CAT_TYPE type);

	// メニュー選択時の処理
	void UpdateYes(void);	// Yes
	void UpdateNo(void);	// No

	// 選択処理
	void UpdateSelect(void);
	void MouseSelect(void);		// マウス選択
	void PadSelect(void);		// パッド選択

	// 猫更新処理
	void UpdateCat(void);
};

