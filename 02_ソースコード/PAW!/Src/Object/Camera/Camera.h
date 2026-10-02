#pragma once

#include <DxLib.h>

#include "../Component/Component.h"
#include "../Component/Transform/Transform.h"

class Transform;
class Player;

class Camera : public Component
{
public:

	enum class MODE
	{
		NONE,
		FIXED,	// 固定カメラ
		FREE,	// フリーカメラ
		FOLLOW,	// 指定の物体に追従
	};

public:

	// 視点のしきい値(マウス)
	static constexpr float THRESHOLD = 1.5f;

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.09f;

public:

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void PreDraw(void) override;	// 描画前

public:

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// モード変更
	void ChangeMode(MODE mode) { mode_ = mode; }

	// 追従対象
	void SetTarget(Transform* targetTransform) { targetTransform_ = targetTransform; }

	// プレイヤーの情報をもらう　※プレイヤーの場合
	void SetPlayer(Player* player) { player_ = player; }

	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

private:

	// 追従対象からカメラへの相対座標
	static constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS = { 0.0f, 150.0f, -400.0f };

	// 追従対象から注視点への相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 150.0f, 200.0f };

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 20.0f;
	static constexpr float VIEW_FAR = 5000.0f;

	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP_RAD = -60.0f * (DX_PI_F / 180.0f);	// 上上限
	static constexpr float LIMIT_X_DW_RAD = 50.0f * (DX_PI_F / 180.0f);		// 下上限

	// 感度
	static constexpr float MOUSE_SENSITIVITY = 0.003f;	// マウス
	static constexpr float PAD_SENSITIVITY = 0.03f;		// パッド

	// デッドゾーン
	static constexpr float DEAD_ZONE = 0.2f; 

	// 回転速度
	static constexpr float START_DEG = 5.0f;	// 最小
	static constexpr float END_DEG = 100.0f;	// 最大

	// スティック正規化用
	static constexpr float STICK_AXIS_MAX = 1000.0f;

private:

	// Transform
	Transform* transform_ = nullptr;

	// 追従対象Transform
	Transform* targetTransform_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;

private:

	// カメラのモード
	MODE mode_ = MODE::FOLLOW;

	// 追従対象の座標
	VECTOR targetPos_{};

	// マウス画像
	int mousePosX_ = 0;
	int mousePosY_ = 0;

	// 移動時の上下揺らし用カウント
	float angleMoveCount = 0.0f;

private:

	// フォローモードの更新処理
	void UpdateFollow(void);

	// 描画前のカメラ設定
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollow(void);

	// カメラ操作		true = 視点操作(上下)上限有り / false = 視点操作(上下)上限なし
	void ProcessRot(bool isLimit);

	// カメラ回転(キーボード)
	void RotKeyboard(bool isLimit);

	// カメラ回転(ゲームパッド)
	void RotGamePad(bool isLimit);

	// カメラ回転(マウス)
	void RotMouse(bool isLimit);

	// 注視点の移動
	void TargetPosUpdate(MATRIX mat);
	// カメラの移動
	void CameraPosUpdate(MATRIX mat);
};
