#pragma once

#include <DxLib.h>

#include "../../../Component/Component.h"
#include "../../../Component/Transform/Transform.h"
#include "Attack1Info.h"

// 前方宣言
class Transform;
class SphereCollider;

class Attack1 : public Component
{
public:
	// 攻撃1の半径
	static constexpr float ATTACK1_RADIUS = 80.0f;

public:
	Attack1(void);				// コンストラクタ

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新

public:
	// プレイヤー状態を取得
	ATTACK1_STATE GetState(void) { return stateCtrl_.state_; }

	// 攻撃を開始
	void StartAttack(VECTOR pos);

private:
	// 状態別更新処理
	void UpdateState(void);

	// 行動状態
	// 状態別初期化
	static void InitIdle(Attack1& attack1);		// 待機
	static void InitMove(Attack1& attack1);		// 移動
	static void InitEnd(Attack1& attack1);		// 終了
	// 状態別更新
	static void UpdateIdle(Attack1& attack1);	// 待機
	static void UpdateMove(Attack1& attack1);	// 移動
	static void UpdateEnd(Attack1& attack1);	// 終了
	// 状態を変更させる
	void ChangeState(ATTACK1_STATE state);

private:
	// コンポーネント追加
	void AddComponets(void);

	// 移動処理
	void Move(void);

	// 衝突判定の検知
	void OnCollision(Collider* other) override;

private:
	// コンポーネント
	Transform* transform_ = nullptr;
	SphereCollider* sphereCollder_ = nullptr;

private:
	// 猫ロボット情報
	attack1Info info_;

	// 猫ロボット状態情報
	attack1StateCtrl stateCtrl_;
};

