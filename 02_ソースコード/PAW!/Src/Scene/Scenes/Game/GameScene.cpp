#include <algorithm>
#include <EffekseerForDXLib.h>

#include "../../../Application.h"
#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Pause/Pause.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/MouseCursor/MouseCursor.h"
#include "../../../Math/Math.h"
#include "../../../Object/ObjectManager.h"
#include "../../../Object/Tag.h"
#include "../../../Object/Player/Player.h"
#include "../../../Object/Enemy/Rat/Rat.h"
#include "../../../Object/Camera/Camera.h"
#include "../../../Object/Camera/CameraUtility.h"
#include "../../../Object/Grid/Grid.h"

#include "GameScene.h"

GameScene::GameScene(void)
{
	// マウスの表示を消す
	MouseCursor::GetInstance()->SetMouseDraw(false);
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// オブジェクトマネージャー初期化
	objectManger_->Init();

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	// BGM再生
	//AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME_1);
}

void GameScene::Load(void)
{
	// オブジェクトマネージャーの生成
	objectManger_ = new ObjectManager();

	// カメラの作成
	CameraCreate();

	// プレイヤーの作成
	PlayerCreate();

	// ねずみの作成
	RatCreate();

	// 各オブジェクトのロード
	objectManger_->Load();

	// サウンド読みこみ
	//AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);
}

void GameScene::LoadEnd(void)
{
	Init();

	// 参照がほしいクラスにポインタを渡す
	SetUp();
}

void GameScene::Update(void)
{
#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_C))
	{
		// ステージクリアにする
		SceneManager::GetInstance()->TrueGameClear();
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_O))
	{
		// ゲームオーバーにする
		SceneManager::GetInstance()->TrueGameOver();
	}

#endif // _DEBUG

	// オブジェクトの更新
	objectManger_->Update();

	// Effekseerにより再生中のエフェクトを更新する
	UpdateEffekseer3D();

	// ポーズシーン遷移
	ChangePauseScene();

	// ゲームクリア遷移
	ChangeGameClearScene();

	// ゲームオーバー遷移
	ChangeGameOverScene();
}

void GameScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	// グリッド描画
	grid_->Draw();

	// オブジェクトの3D描画
	objectManger_->Draw3D();

	// Effekseerにより再生中のエフェクトを描画する
	DrawEffekseer3D();

	// オブジェクトの2D描画
	objectManger_->Draw2D();

#ifdef _DEBUG

	DrawString(0, 0, "Game", 0xffffff);

#endif // _DEBUG
}

void GameScene::Release(void)
{
	// グリッド解放
	grid_->Release();
	delete grid_;

	// オブジェクトマネージャー削除
	delete objectManger_;

	// 音の解放
	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
}

void GameScene::CameraCreate(void)
{
	// カメラ生成
	auto cameraObj = objectManger_->CreateObject();

	// タグの付与
	cameraObj->SetTagAndPriority(TAG_3D::CAMERA);

	// カメラコンポーネントの付与
	auto camera = cameraObj->AddComponent<Camera>();

	// カメラのモードを変更
	camera->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::PlayerCreate(void)
{
	// プレイヤー生成
	auto player = objectManger_->CreateObject();

	// タグを付与
	player->SetTagAndPriority(TAG_3D::PLAYER,TAG_2D::PLAYER);

	// プレイヤー機能セット
	player->AddComponent<Player>();
}

void GameScene::RatCreate(void)
{
	// プレイヤー生成
	auto rat = objectManger_->CreateObject();

	// タグを付与
	rat->SetTagAndPriority(TAG_3D::RAT);

	// プレイヤー機能セット
	rat->AddComponent<Rat>();
}

void GameScene::SetUp(void)
{
	 // プレイヤーを取得
	auto player = objectManger_->FindComponentWithTag<Player>(TAG_3D::PLAYER);

	// カメラの取得
	auto camera = objectManger_->FindComponentWithTag<Camera>(TAG_3D::CAMERA);

	// プレイヤーの情報をカメラに設定
	camera->SetTarget(player->GetTransform());
	camera->SetPlayer(player);

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(camera);

	// ねずみに読み取り専用でプレイヤー座標を渡す
	auto rat = objectManger_->FindComponentWithTag<Rat>(TAG_3D::RAT);
	rat->SetPlayerPos(&player->GetTransform()->pos_);
}

void GameScene::ChangePauseScene(void)
{
	if (InputManager::GetInstance()->PauseButton())
	{
		// ポーズ画面を開いたサウンド
		//AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}
}

void GameScene::ChangeGameClearScene(void)
{
	if (SceneManager::GetInstance()->GetIsClear())
	{
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		// ゲームクリアシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameClear>(), CLEAR);
		return;
	}
}

void GameScene::ChangeGameOverScene(void)
{
	if (SceneManager::GetInstance()->GetIsOver())
	{
		// ゲームオーバーシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameOver>(), OVER);
		return;
	}
}
