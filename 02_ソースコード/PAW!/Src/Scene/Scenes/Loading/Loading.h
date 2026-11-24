#pragma once

#include "../../../Math/Vector2.h"

class Loading
{
public:
	// 現在描画している画像の種類
	enum NOW_TYPE
	{
		LOADING_0,
		LOADING_1,
		LOADING_2,
		LOADING_3,

		MAX,
	};

public:
	Loading(void);				// コンストラクタ

	void Init(void);		// 初期化
	void Load(void);		// 読み込み
	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

public:
	void StartAsyncLoad(void);	// 非同期ロードの開始
	void EndAsyncLoad(void);	// 非同期ロードの終了

	bool IsLoading(void) { return isLoading_; }	// ロード中かを返す

private:
	int handles_[static_cast<int>(NOW_TYPE::MAX)];	// 画像ハンドル

	Vector2 pos_;	// 座標

	NOW_TYPE nowType_;	// 現在描画している画像の種類

	bool isLoading_;	// ロード中の判定用
	
	int loadTimer_;	// 最低でもロード画面を表示する時間の範囲
};
