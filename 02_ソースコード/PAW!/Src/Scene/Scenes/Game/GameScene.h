#pragma once

#include <vector>
#include <string>
#include <memory>
#include <DxLib.h>

#include "../../SceneBase.h"

class ObjectManager;
class Grid;

class GameScene : public SceneBase
{
public:

	GameScene(void);				// コンストラクタ
	~GameScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// オブジェクトマネージャー
	ObjectManager* objectManger_ = nullptr;

	// グリッド線
	Grid* grid_  = nullptr;

private:

	// オブジェクト作成
	// カメラの作成
	void CameraCreate(void);	
	// プレイヤーの作成
	void PlayerCreate(void);	

	// 参照系
	void SetUp(void);

	// シーン遷移
	void ChangePauseScene(void);		// ポーズシーン
	void ChangeGameClearScene(void);	// ゲームクリア
	void ChangeGameOverScene(void);		// ゲームオーバー
};
