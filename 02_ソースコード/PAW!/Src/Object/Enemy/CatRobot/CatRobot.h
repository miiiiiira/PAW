#pragma once

#include "../../Component/Component.h"
#include "../../Component/Transform/Transform.h"
#include "CatRobotInfo.h"

// 前方宣言
class Transform;
class Render3D;
class Animation;
class CapsuleCollider;
class SphereCollider;
class Attack1;

class CatRobot : public Component
{
public:
	CatRobot(void);				// コンストラクタ

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// 状態を取得
	CAT_ROBOT_STATE GetState(void)const { return stateCtrl_.state_; }

	// 状態を変更させる
	void ChangeState(CAT_ROBOT_STATE state);

	// プレイヤーのTransformポインタを取得
	void SetPlayerTransform(const Transform* transform) { playerTransform_ = transform; }

private:
	// 状態別更新処理
	void UpdateState(void);

	// 行動状態
	// 状態別初期化
	static void InitIdle(CatRobot& catRobot);		// 待機
	static void InitMove(CatRobot& catRobot);		// 移動
	static void InitAttack(CatRobot& catRobot);		// 攻撃
	static void InitChance(CatRobot& catRobot);		// プレイヤーの攻撃チャンス
	static void InitDead(CatRobot& catRobot);		// 死亡
	static void InitEnd(CatRobot& catRobot);		// 終了
	// 状態別更新
	static void UpdateIdle(CatRobot& catRobot);		// 待機
	static void UpdateMove(CatRobot& catRobot);		// 移動
	static void UpdateAttack(CatRobot& catRobot);	// 攻撃
	static void UpdateChance(CatRobot& catRobot);	// プレイヤーの攻撃チャンス
	static void UpdateDead(CatRobot& catRobot);		// 死亡
	static void UpdateEnd(CatRobot& catRobot);		// 終了

	// 攻撃状態
	// 状態別初期化
	static void InitAttackSelect(CatRobot& catRobot);		// 攻撃選択
	static void InitAttack1(CatRobot& catRobot);			// 食材爆撃
	static void InitAttack2(CatRobot& catRobot);			// お皿攻撃上下
	static void InitAttack3(CatRobot& catRobot);			// お皿攻撃左右
	static void InitAttack4(CatRobot& catRobot);			// ナイフ攻撃
	static void InitAttackEnd(CatRobot& catRobot);			// 攻撃終了
	// 状態別更新
	static void UpdateAttackSelect(CatRobot& catRobot);		// 攻撃選択
	static void UpdateAttack1(CatRobot& catRobot);			// 食材爆撃
	static void UpdateAttack2(CatRobot& catRobot);			// お皿攻撃上下
	static void UpdateAttack3(CatRobot& catRobot);			// お皿攻撃左右
	static void UpdateAttack4(CatRobot& catRobot);			// ナイフ攻撃
	static void UpdateAttackEnd(CatRobot& catRobot);		// 攻撃終了
	// 攻撃状態を変更させる
	void ChangeAttackState(CAT_ROBOT_ATTACK_STATE state);

	// プレイヤーの攻撃チャンス状態
	// 状態別初期化
	static void InitChanceStart(CatRobot& catRobot);	// 攻撃を受けられる位置まで移動する
	static void InitChanceStun(CatRobot& catRobot);		// スタンして無防備になる(プレイヤーの攻撃チャンス)
	static void InitChanceEnd(CatRobot& catRobot);		// スタンから回復し、通常行動へ戻る
	// 状態別更新
	static void UpdateChanceStart(CatRobot& catRobot);	// 攻撃を受けられる位置まで移動する
	static void UpdateChanceStun(CatRobot& catRobot);	// スタンして無防備になる(プレイヤーの攻撃チャンス)
	static void UpdateChanceEnd(CatRobot& catRobot);	// スタンから回復し、通常行動へ戻る
	// プレイヤーの攻撃チャンス状態を変更させる
	void ChangeChanceState(CAT_ROBOT_CHANCE_STATE state);

private:
	// コンポーネント追加
	void AddComponets(void);

	// 重力
	void ApplyGravity(void);

	// 無敵時間を更新
	void UpdateInvincible(void);

	// 移動処理
	void Move(void);

	// 移動方向を算出 
	// 引数をtrueにするとプレイヤーから離れる方向を算出
	void GetDirectionToPlayer(bool isRetreat = false);

	// 攻撃1の座標決め
	void ResetAttack1Pos(void);
	// 攻撃の範囲が重なっていなかったらtrueを返す（円で見る）
	bool IsFarEnough(VECTOR pos, float rad, int i = 0);

	// ダメージを与える
	void SetDamage(void);

	// 衝突判定の検知
	void OnCollision(Collider* other) override;

private:
	// コンポーネント
	Transform* transform_ = nullptr;
	CapsuleCollider* capsuleCollder_ = nullptr;
	Render3D* render3D_ = nullptr;
	Animation* animation_ = nullptr;

	// 攻撃コンポーネント
	std::vector<Attack1*> attack1_;

	// プレイヤーTransform
	const Transform* playerTransform_ = nullptr;

private:
	// 猫ロボット情報
	catRobotInfo info_;

	// 猫ロボット状態情報
	catRobotStateCtrl stateCtrl_;
};

