#pragma once
#include "CollisionInfo.h"
#include "../Object/Component/Component.h"
#include "../Object/Component/Transform/Transform.h"

// コライダー基底クラス
class Collider : public Component
{
public:

	// 仮想デストラクタ
	virtual ~Collider() {}

	// 初期化
	virtual void Init() override;

	// 更新
	virtual void Update() override;

	// 当たり判定(派生クラス側で実装)
	virtual bool CheckCollision(Collider* other) = 0;

	// 衝突した場合、方向と重なり量も返す
	CollisionInfo GetCollisionInfo(void) { return collisionInfo_; };

	// 所有オブジェクト取得
	Object* GetOwner() { return owner_; }

	// タグを設定
	void SetCollisionTag(CollisionTag tag) { tag_ = tag; }

	// タグを取得
	CollisionTag GetCollisionTag(void) { return tag_; }

	// 逆質量。0なら固定、1なら通常の動く物体
	void SetInverseMass(float value) { inverseMass_ = value; }

	float GetInverseMass() const { return inverseMass_; }
	
#ifdef _DEBUG
	// 当たり判定の形状をデバッグ表示する
	virtual void DebugDraw() const {}
#endif // _DEBUG

protected:
	
	// Transform参照
	Transform* transform_ = nullptr;

	// 当たり判定の分類タグ
	CollisionTag tag_ = CollisionTag::NONE;

	// 当たり判定の情報
	CollisionInfo collisionInfo_;

	// 1.0f = 通常の移動可能な物体 
	// 0.5f = 押し出しの分担を少なくしたい物体 
	// 0.0f = 固定オブジェクト
	float inverseMass_ = 1.0f;
};
