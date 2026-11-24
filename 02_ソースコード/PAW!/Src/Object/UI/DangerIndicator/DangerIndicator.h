#pragma once

#include <vector>
#include "../../../Math/Vector2.h"

class CatRobot;
class DangerIndicator
{
public:
	// 現在描画している画像の種類
	enum ARROW_TYPE
	{
		ARROW_1,
		ARROW_2,
		ARROW_3,

		ARROW_MAX,
	};

public:
	DangerIndicator(CatRobot& catRobot);		// コンストラクタ
	~DangerIndicator(void);		// デストラクタ

	void Init(void);			// 初期化
	void Update(void);			// 更新
	void Load(void);			// 読み込み	
	void Draw(void);			// 描画
	void Release(void);			// 解放

private:
	CatRobot& catRobot_;

private:
	// 画像ハンドル
	int handles_[ARROW_TYPE::ARROW_MAX];

	// 現在描画している画像の種類
	ARROW_TYPE arrowType_ = ARROW_MAX;

	// 描画カウンター
	int drawCounter_ = 0;

	// 一度でもカウンタが動いたか
	bool isOnce_ = false;

private:
	// お皿攻撃の確認
	void CheckPlateAttack(void);

	// カトラリー攻撃の確認
	void CheckCutleryAttack(void);

	// 描画する画像の種類を選択
	void SelectArrowType(VECTOR pos);
};

