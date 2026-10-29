#include <algorithm>
#include <EffekseerForDXLib.h>

#include "../../../Application.h"
#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Pause/Pause.h"
#include "../../../Collision/CollisionMath.h"
#include "../../../Collision/CollisionManager.h"
#include "../../../Input/MouseCursor/MouseCursor.h"
#include "../../../Math/Math.h"
#include "../../../Object/ObjectManager.h"
#include "../../../Object/Tag.h"
#include "../../../Object/Player/Player.h"
#include "../../../Object/Enemy/Rat/Rat.h"
#include "../../../Object/Enemy/CatRobot/CatRobot.h"
#include "../../../Object/Camera/Camera.h"
#include "../../../Object/Camera/CameraUtility.h"
#include "../../../Object/Enemy/EnemyCommander.h"
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
	objectManager_->Init();

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	// 敵司令官の初期化
	// ネズミの参照を渡す
	auto rats = objectManager_->FindComponentWithTagAll<Rat>(TAG_3D::RAT);
	auto catRobot = objectManager_->FindComponentWithTag<CatRobot>(TAG_3D::CAT_ROBOT);
	enemyCommander_->Init(rats,catRobot);

	// BGM再生
	//AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME_1);
}

void GameScene::Load(void)
{
	// オブジェクトマネージャーの生成
	objectManager_ = new ObjectManager();

	// カメラの作成
	CameraCreate();

	// プレイヤーの作成
	PlayerCreate();

	// ネズミの作成
	for (int i = 0; i < 1; ++i)
	{
		RatCreate();
	}

	// 猫ロボットの作成
	CatRobotCreate();

	// 各オブジェクトのロード
	objectManager_->Load();

	// 当たり判定のマネジャー生成
	CollisionManager::CreateInstance();

	// 敵司令官の生成
	enemyCommander_ = new EnemyCommander();

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

	// 敵司令官の更新
	enemyCommander_->Update();

	// 毎フレーム登録をリセット
	CollisionManager::GetInstance()->Clear();

	// オブジェクトの更新
	objectManager_->Update();

	// 全ての登録コライダーを検査
	CollisionManager::GetInstance()->CheckAll();

	// Effekseerにより再生中のエフェクトを更新する
	UpdateEffekseer3D();

	// ポーズシーン遷移
	if (ChangePauseScene())return;

	// ゲームクリア遷移
	if (ChangeGameClearScene())return;

	// ゲームオーバー遷移
	if (ChangeGameOverScene())return;
}

void GameScene::Draw(void)
{
	// オブジェクトの描画前
	objectManager_->PreDraw();

	// グリッド描画
	grid_->Draw();

	// オブジェクトの3D描画
	objectManager_->Draw3D();

	// Effekseerにより再生中のエフェクトを描画する
	DrawEffekseer3D();

	// オブジェクトの2D描画
	objectManager_->Draw2D();

#ifdef _DEBUG

	DrawString(0, 0, "Game", 0xffffff);
	CollisionManager::GetInstance()->DrawDebug();

#endif // _DEBUG
}

void GameScene::Release(void)
{
	// 敵司令官の解放
	enemyCommander_->Release();
	delete enemyCommander_;
	enemyCommander_ = nullptr;

	// グリッド解放
	grid_->Release();
	delete grid_;

	// 当たり判定のマネジャー削除
	CollisionManager::GetInstance()->Clear();
	CollisionManager::DeleteInstance();

	// オブジェクトマネージャー削除
	delete objectManager_;
	objectManager_ = nullptr;

	// 音の解放
	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
}

void GameScene::CameraCreate(void)
{
	// カメラ生成
	auto cameraObj = objectManager_->CreateObject();

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
	auto player = objectManager_->CreateObject();

	// タグを付与
	player->SetTagAndPriority(TAG_3D::PLAYER,TAG_2D::PLAYER);

	// プレイヤー機能セット
	player->AddComponent<Player>();
}

void GameScene::RatCreate(void)
{
	// ネズミ生成
	auto rat = objectManager_->CreateObject();

	// タグを付与
	rat->SetTagAndPriority(TAG_3D::RAT);

	// ネズミ機能セット
	rat->AddComponent<Rat>();
}

void GameScene::CatRobotCreate(void)
{
	// 猫ロボット生成
	auto catRobot = objectManager_->CreateObject();

	// タグを付与
	catRobot->SetTagAndPriority(TAG_3D::CAT_ROBOT);

	// 猫ロボット機能セット
	catRobot->AddComponent<CatRobot>();
}

void GameScene::SetUp(void)
{
	// ポインタを取得
	// プレイヤー
	auto player = objectManager_->FindComponentWithTag<Player>(TAG_3D::PLAYER);
	// カメラ
	auto camera = objectManager_->FindComponentWithTag<Camera>(TAG_3D::CAMERA);
	// ネズミ
	auto rats = objectManager_->FindComponentWithTagAll<Rat>(TAG_3D::RAT);
	// 猫ロボット
	auto catRobot = objectManager_->FindComponentWithTag<CatRobot>(TAG_3D::CAT_ROBOT);

	// プレイヤーのTransformをカメラに設定
	camera->SetTarget(player->GetTransform());

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(camera);

	for (const auto& rat : rats)
	{
		// プレイヤーにネズミTransformを渡す
		player->SetRatPoint(rat->GetTransform());

		// ネズミにプレイヤーTransformを渡す
		rat->SetPlayerTransform(player->GetTransform());
	}

	// 猫ロボットにプレイヤーTransformを渡す
	catRobot->SetPlayerTransform(player->GetTransform());
}

bool GameScene::ChangePauseScene(void)
{
	if (InputManager::GetInstance()->PauseButton())
	{
		// ポーズ画面を開いたサウンド
		//AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return true;
	}

	return false;
}

bool GameScene::ChangeGameClearScene(void)
{
	if (SceneManager::GetInstance()->GetIsClear())
	{
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		// ゲームクリアシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameClear>(), CLEAR);
		return true;
	}

	return false;
}

bool GameScene::ChangeGameOverScene(void)
{
	if (SceneManager::GetInstance()->GetIsOver())
	{
		// ゲームオーバーシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameOver>(), OVER);
		return true;
	}

	return false;
}
