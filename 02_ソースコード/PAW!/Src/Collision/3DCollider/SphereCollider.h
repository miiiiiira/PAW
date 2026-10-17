#pragma once
#include "../Collider.h"
#include <cmath>

// 球体コライダー
class SphereCollider : public Collider
{
public:

    // オブジェクトの座標からの中心位置のずれ
	VECTOR centerOffset = VGet(0.0f, 0.0f, 0.0f);

	// 球体の半径
	float radius_ = 10.0f;

    // 球体の中心位置をワールド座標で取得する
    VECTOR GetCenter() const
    {
        return VAdd(transform_->pos_, centerOffset);
    }

    // 球体の半径を取得する
    float GetRadius() const
    {
        return radius_;
    }

    // 球体の中心位置を更新する
    void SetCenterOffset(const VECTOR& offset)
    {
        centerOffset = offset;
    }

    // 球体の半径を変更する
    void SetRadius(float value)
    {
        // 0以下の値に設定されないようにする
        if (value > 0.0f)
            radius_ = value;
    }

    // 相手のコライダーと衝突しているかを判定する
    bool CheckCollision(Collider* other) override;

    // 相手のコライダーとの衝突を調べ、衝突情報を取得する
    bool GetCollisionInfo(Collider* other, CollisionInfo& info) override;

#ifdef _DEBUG
    // 球体の当たり判定をデバッグ表示する
    void DebugDraw() const override;
#endif // _DEBUG

};
