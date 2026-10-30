#pragma once
#include "../Collider.h"

// 円柱コライダー(軸はワールドのY軸に平行)
class CylinderCollider : public Collider
{
public:

    // オブジェクトの座標からの中心位置のずれ
    VECTOR centerOffset = VGet(0.0f, 0.0f, 0.0f);

    // 円柱の半径
    float radius_ = 10.0f;

    // 円柱の高さ(全高)
    float height_ = 30.0f;

    // 円柱の中心位置をワールド座標で取得する
    VECTOR GetCenter() const;

    // Transformを渡す
    void SetTransform(Transform* transform)
    {
        transform_ = transform;
    }

    // 円柱の半径を取得する
    float GetRadius() const
    {
        return radius_;
    }

    // 円柱の高さを取得する
    float GetHeight() const
    {
        return height_;
    }

    // 円柱の中心位置を更新する
    void SetCenterOffset(const VECTOR& offset)
    {
        centerOffset = offset;
    }

    // 円柱の半径を変更する
    void SetRadius(float value)
    {
        // 0以下の値に設定されないようにする
        if (value > 0.0f)
            radius_ = value;
    }

    // 円柱の高さを変更する
    void SetHeight(float value)
    {
        // 0以下の値に設定されないようにする
        if (value > 0.0f)
            height_ = value;
    }

    // 相手のコライダーと衝突しているかを判定する
    bool CheckCollision(Collider* other) override;

#ifdef _DEBUG
    // 円柱の当たり判定をデバッグ表示する
    void DebugDraw() const override;
#endif // _DEBUG

};
