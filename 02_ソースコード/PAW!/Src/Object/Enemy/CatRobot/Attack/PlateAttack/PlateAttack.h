#pragma once

#include <DxLib.h>

#include "../../../../Component/Component.h"
#include "../../../../Component/Transform/Transform.h"
#include "PlateAttackInfo.h"

// 前方宣言
class Transform;
class CylinderCollider;

class PlateAttack : public Component
{
public:
	// 半径
	static constexpr float PLATE_RADIUS = 400.0f;
	static constexpr float PLATE_HEIGHT = 50.0f;

public:
	PlateAttack(void);				// コンストラクタ

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// 状態を取得
	PLATE_ATTACK_STATE GetState(void) { return stateCtrl_.state_; }

	// 攻撃を開始
	void StartAttack(VECTOR pos, VECTOR moveDir, float moveDistance, int idleCount);

private:
	// 状態別更新処理
	void UpdateState(void);

	// 行動状態
	// 状態別初期化
	static void InitIdle(PlateAttack& plateAttack);	// 待機
	static void InitMove(PlateAttack& plateAttack);		// 移動
	static void InitEnd(PlateAttack& plateAttack);		// 終了
	// 状態別更新
	static void UpdateIdle(PlateAttack& plateAttack);	// 待機
	static void UpdateMove(PlateAttack& plateAttack);	// 移動
	static void UpdateEnd(PlateAttack& plateAttack);	// 終了
	// 状態を変更させる
	void ChangeState(PLATE_ATTACK_STATE state);

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
	CylinderCollider* cylinderCollider_ = nullptr;

private:
	// 攻撃情報
	attack2Info info_;

	// 状態情報
	attack2StateCtrl stateCtrl_;
};
