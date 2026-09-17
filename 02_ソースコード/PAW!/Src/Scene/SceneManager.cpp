#include "SceneManager.h"

#include "SceneBase.h"
#include "Scenes/Loading/Loading.h"
#include "Scenes/Title/TitleScene.h"
#include "../Application.h"

#include "../Input/InputManager.h"
//#include "../Common/FrameRenderer/FrameRenderer.h"
//#include "../Common/MouseCursor/MouseCursor.h"

SceneManager* SceneManager::instance_ = nullptr;

SceneManager::SceneManager(void)
{
	scenes_.clear();
	load_ = nullptr;
	isGameEnd_ = false;
}

SceneManager::~SceneManager(void)
{
}

void SceneManager::Init(void)
{
	// ロード画面生成
	load_ = new Loading();
	load_->Init();
	load_->Load();

	// フェーダークラスを生成
	Fader::GetInstance()->CreateInstance();
	Fader::GetInstance()->Init();

	//// マウスカーソルクラスを生成
	//MouseCursor::CreateInstance();
	//MouseCursor::GetInstance()->Load();
	//MouseCursor::GetInstance()->Init();

	//// フレーム画像のロード
	//FrameRenderer::Load();

	// 3D情報の初期化
	Init3D();

	// ゲームクリア判定用初期化
	isClear_ = false;
	// ゲームオーバー判定用初期化
	isOver_ = false;

	// 最初はタイトル画面から
	ChangeScene(std::make_shared<TitleScene>());

	// 既存のマウスカーソルは描画しない
	SetMouseDispFlag(false);
}

void SceneManager::Init3D(void)
{
	// 背景色設定
	SetBackgroundColor(0, 0, 0);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// 標準ライトをディレクショナルライトにする
	ChangeLightTypeDir(VGet(-0.5f, -0.3f, -1.0f));

	SetLightDifColor(GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	
	// フォグ設定
	SetFogEnable(true);
	// フォグの色
	SetFogColor(0, 0, 0);
	// フォグを発生させる奥行きの最小、最大距離
	SetFogStartEnd(FOG_START, FOG_END);
	// フォグの密度を設定
	SetFogDensity(1.0f);
}

// 更新
void SceneManager::Update(void)
{
	// シーンがなければ終了
	if (scenes_.empty()) { return; }

	// ロード中
	if (load_->IsLoading())
	{
		// ロード更新
		load_->Update();

		// ロードの更新が終了していたら
		if (!load_->IsLoading())
		{
			// ロード後の初期化
			scenes_.back()->LoadEnd();
		}
	}
	// 通常の更新処理
	else
	{
		// フェードアウト中はシーン更新の停止
		if (changeState_ != CHANGE_STATE::FADE_OUT)
		{
			// 現在のシーンの更新
			scenes_.back()->Update();

			//// マウスカーソルの座標更新処理
			//MouseCursor::GetInstance()->Update();
		}
	}

	// フェードを更新
	Fader::GetInstance()->Update();

	// シーンの状態
	switch (changeState_)
	{
	case SceneManager::CHANGE_STATE::NONE:
		break;
		// フェードアウト待ち
	case SceneManager::CHANGE_STATE::FADE_OUT:
		if (Fader::GetInstance()->GetState() == Fader::STATE::END)
		{
			// 真っ暗な状態でシーン切り替え
			if (isJumpScene_)
			{
				// 強制的に特定のシーンに飛ぶ
				JumpScene(nextScene_);
			}
			else
			{
				// 末尾のものを新しいシーンへ変更
				ChangeScene(nextScene_);
			}
			changeState_ = CHANGE_STATE::LOADING;
		}
		break;
		// ロード待ち
	case SceneManager::CHANGE_STATE::LOADING:
		if (!load_->IsLoading())
		{
			Fader::GetInstance()->SetFade(Fader::STATE::FADE_IN);
			changeState_ = CHANGE_STATE::FADE_IN;
		}
		break;
		// フェードイン待ち
	case SceneManager::CHANGE_STATE::FADE_IN:
		if (Fader::GetInstance()->GetState() == Fader::STATE::END)
		{
			Fader::GetInstance()->Init();
			changeState_ = CHANGE_STATE::NONE;
			nextScene_ = nullptr;
		}
		break;
	default:
		break;
	}

}

void SceneManager::Draw(void)
{
	// ロード中ならロード画面を描画
	if (load_->IsLoading())
	{
		// ロードの描画
		load_->Draw();
	}
	// 通常の更新
	else
	{
		// 描画先グラフィック領域の指定
		SetDrawScreen(DX_SCREEN_BACK);
		// 画面を初期化
		ClearDrawScreen();

		// 積まれているもの全てを描画する
		for (auto& scene : scenes_)
		{
			// シーンの描画
			scene->Draw();
		}

		//// マウスカーソルの描画
		//MouseCursor::GetInstance()->Draw();
	}

	// フェード描画
	Fader::GetInstance()->Draw();
}

void SceneManager::Delete(void)
{
	// 全てのシーンの解放・削除
	for (auto& scene : scenes_) { scene->Release(); }
	scenes_.clear();

	//FrameRenderer::Release();

	// フェーダー解放
	Fader::GetInstance()->DeleteInstance();

	//// マウスカーソル解放
	//MouseCursor::GetInstance()->Destroy();

	// ロード画面の削除
	load_->Release();
	delete load_;
}

void SceneManager::NextChangeScene(std::shared_ptr<SceneBase> scene, SCENE_TAG sceneTag, bool isJumpScne, Fader::TYPE type)
{
	// ジャンプシーンフラグを設定する
	isJumpScene_ = isJumpScne;

	// シーンタグを設定
	nowSceneTag_ = sceneTag;

	// 遷移するシーンを予約する
	nextScene_ = scene;

	// 状態をフェードに
	changeState_ = CHANGE_STATE::FADE_OUT;
	
	// フェードアウト
	Fader::GetInstance()->SetFade(Fader::STATE::FADE_OUT, type);
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase>scene)
{
	// シーンが空か？
	if (scenes_.empty())
	{
		//空なので新しく入れる
		scenes_.push_back(scene);
	}
	else
	{
		//末尾のものを新しい物に入れ替える
		scenes_.back()->Release();
		scenes_.back() = scene;
	}

	// ゲームクリア判定用初期化
	isClear_ = false;
	// ゲームオーバー判定用初期化
	isOver_ = false;

	// 読み込み(非同期)
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	//新しく積むのでもともと入っている奴はまだ削除されない
	scenes_.push_back(scene);
	scenes_.back()->Load();
	scenes_.back()->Init();
}

void SceneManager::PopScene(void)
{
	//積んであるものを消して、もともとあったものを末尾にする
	if (scenes_.size() > 0)
	{
		scenes_.back()->Release();
		scenes_.pop_back();
	}
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
	// 全て解放
	for (auto& scene : scenes_) { scene->Release(); }
	scenes_.clear();

	// 新しく積む
	scenes_.push_back(scene);

	// ゲームクリア判定用初期化
	isClear_ = false;
	// ゲームオーバー判定用初期化
	isOver_ = false;

	// 読み込み(非同期)
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();
}

void SceneManager::TrueGameClear(void)
{
	// フラグを立てる
	isClear_ = true;
}

void SceneManager::TrueGameOver(void)
{
	// フラグを立てる
	isOver_ = true;
}

void SceneManager::ResetGame(void)
{
}
