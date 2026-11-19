#pragma once

#include <DxLib.h>

#include "../../../../Component/Component.h"
#include "../../../../Component/Transform/Transform.h"
#include "CutleryAttackInfo.h"

// 前方宣言
class Transform;
class Render3D;
class CapsuleCollider;

class CutleryAttack : public Component
{
public:
	CutleryAttack(void);		// コンストラクタ

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// 状態を取得
	CUTLERY_ATTACK_STATE GetState(void) { return stateCtrl_.state_; }

	// 攻撃を開始
	void StartAttack(VECTOR pos, VECTOR moveDir, float moveDistance, int idleCount);

private:
	// 状態別更新処理
	void UpdateState(void);

	// 行動状態
	// 状態別初期化
	static void InitIdle(CutleryAttack& cutleryAttack);	// 待機
	static void InitMove(CutleryAttack& cutleryAttack);	// 移動
	static void InitEnd(CutleryAttack& cutleryAttack);	// 終了
	// 状態別更新
	static void UpdateIdle(CutleryAttack& cutleryAttack);	// 待機
	static void UpdateMove(CutleryAttack& cutleryAttack);	// 移動
	static void UpdateEnd(CutleryAttack& cutleryAttack);	// 終了
	// 状態を変更させる
	void ChangeState(CUTLERY_ATTACK_STATE state);

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
	Render3D* render3D_ = nullptr;
	CapsuleCollider* capsuleCollider_ = nullptr;

private:
	// 攻撃情報
	attack3Info info_;

	// 状態情報
	attack3StateCtrl stateCtrl_;
};