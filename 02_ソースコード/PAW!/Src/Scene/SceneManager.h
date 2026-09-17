#pragma once

#include <list>
#include <memory>
#include <chrono>
#include <DxLib.h>
#include "SceneTag.h"
#include "../System/Fader/Fader.h" 

class SceneBase;
class Loading;
class Fader;

class SceneManager
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new SceneManager(); } };
	static SceneManager* GetInstance(void) { return instance_; };
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	// 静的インスタンス
	static SceneManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// デストラクタも同様
	~SceneManager(void);

	// コピー・ムーブ操作を禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// SceneManager copy = *SceneManager::GetInstance();
	// SceneManager copied(*SceneManager::GetInstance());
	// SceneManager moved = std::move(*SceneManager::GetInstance());

public:

	// 環境光のカラー値、アルファ値
	static constexpr COLOR_F DIF_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };

	void Init(void);	// 初期化
	void Init3D(void);	// 3Dの初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Delete(void);	// リソースの破棄

	// 状態遷移(フェードを挟む)
	void NextChangeScene(std::shared_ptr<SceneBase> scene,SCENE_TAG sceneTag, bool isJumpScne = false, Fader::TYPE type = Fader::TYPE::NORMAL);

	// 状態遷移		遷移させたいシーン,全てのシーンを解放させるか
	void ChangeScene(std::shared_ptr<SceneBase> scene);

	// シーンを新しく積む
	void PushScene(std::shared_ptr<SceneBase> scene);

	// 最後に追加したシーンを削除する。
	void PopScene(void);

	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする。
	void JumpScene(std::shared_ptr<SceneBase> scene);

	// ゲーム終了
	void GameEnd(void) { isGameEnd_ = true; }

	// ゲーム終了取得
	bool GetGameEnd(void) { return isGameEnd_; }

	// ゲームクリアかどうか取得
	const bool GetIsClear(void) { return isClear_; }
	// ゲームオーバーかどうか取得
	const bool GetIsOver(void) { return isOver_; }

	// ゲームクリアのフラグをたてる
	void TrueGameClear(void);
	// ゲームオーバーのフラグをたてる
	void TrueGameOver(void);
	//	ゲームリセット処理
	void ResetGame(void);

	// 現在のシーンタグを渡す
	SCENE_TAG GetNowSceneTag(void) { return nowSceneTag_; }

private:

	// シーンマネージャーの状態
	enum class CHANGE_STATE
	{
		NONE,
		FADE_OUT,
		LOADING,
		FADE_IN
	};

	// フォグのスタート位置終了位置
	static constexpr float FOG_START = 0.0f;
	static constexpr float FOG_END = 3150.0f;

	// シーンマネージャーの状態
	CHANGE_STATE changeState_;

	// ジャンプシーンか否か
	bool isJumpScene_;

	// 各種シーン
	std::list<std::shared_ptr<SceneBase>> scenes_;

	// シーン遷移の予約
	std::shared_ptr<SceneBase> nextScene_;

	// ロード画面
	Loading* load_;

	// ゲーム終了
	bool isGameEnd_;

	// ゲームクリア判定用
	bool isClear_;
	// ゲームオーバー判定用
	bool isOver_;

	// 現在のシーンのタグを保持
	SCENE_TAG nowSceneTag_;
};
