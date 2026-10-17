#include "SphereCollider.h"
#include "CapsuleCollider.h"

#include "../../Object/Object.h"
#include "../../Math/Math.h"
#include "../../Math/MatrixUtility.h"

#include <cmath>

VECTOR SphereCollider::GetCenter() const
{
    // カメラの回転行列
    VECTOR vec = { transform_->angle_.x,transform_->angle_.y,0.0f };
    MATRIX matRot = Matrix::GetMatrixRotateXYZ(vec);

    // 指定の座標にカメラの回転を適用する
    VECTOR localPosRot = VTransform(centerOffset, matRot);

    return VAdd(transform_->pos_, localPosRot);
}

bool SphereCollider::CheckCollision(Collider* other)
{
    // 前期あの判定結果が残らないように初期化する
    collisionInfo_ = CollisionInfo{};

    // 判定に必要な情報がなければ処理しない
    if (!transform_ || !other)
        return false;

    // 自身の球体の中心位置を取得する
    VECTOR centerA = GetCenter();

    // 球体同士の衝突判定
    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        // 相手の座標情報を取得できなければ判定しない
        if (!sphere->GetOwner() || !sphere->GetOwner()->GetComponent<Transform>())
            return false;

        VECTOR centerB = sphere->GetCenter();

        // 二つの球体の中心間の距離を求める
        VECTOR delta = VSub(centerB, centerA);
        float distanceSq = Math::LengthSq(delta);

        // 二つの半径を合計し、衝突判定に使う
        float radiusSum = radius_ + sphere->GetRadius();

        // 中心間の距離が半径の合計以上なら、球体同士は接触していない
        if (distanceSq >= radiusSum * radiusSum)
            return false;

        float distance = sqrtf(distanceSq);

        collisionInfo_.hit = true;

        // 自身の中心から相手の中心へ向かう方向を衝突法線とする
        // 二つの中心がほぼ同じ位置にある場合は、仮の方向を設定する。
        collisionInfo_.normal = distance > 0.0001f ?
            VScale(delta, 1.0f / distance) :
            VGet(1.0f, 0.0f, 0.0f);

        // 球体同士が重なっている深さを求める
        collisionInfo_.penetration = radiusSum - distance;

        // 衝突位置の目安として、重なり部分の中間付近を求める
        collisionInfo_.point = VAdd(centerA, VScale(collisionInfo_.normal, radius_ - collisionInfo_.penetration * 0.5f));

        return true;
    }

    // 球とカプセルの衝突判定
    if (auto cap = dynamic_cast<CapsuleCollider*>(other))
    {
        // 相手の座標情報を取得できなければ判定しない
        if (!cap->GetOwner() || !cap->GetOwner()->GetComponent<Transform>())
            return false;

        float bestPenetration = 0.0f;

        // 相手の全てのカプセルを判定
        for (const auto& capsule : cap->GetCapsules())
        {
            // カプセルの両端をワールド座標に変換する
            VECTOR start = VAdd(cap->GetOwner()->GetComponent<Transform>()->pos_, capsule.startOffset);
            VECTOR end = VAdd(cap->GetOwner()->GetComponent<Transform>()->pos_, capsule.endOffset);

            // 球体の中心に最も近い、カプセルの線分上の点を求める
            VECTOR closest = Math::ClosestPointOnSegment(centerA, start, end);

            // 球体の中心から、カプセル側の最近接点へ向かうベクトル
            VECTOR delta = VSub(closest, centerA);

            float distanceSq = Math::LengthSq(delta);
            float radiusSum = radius_ + capsule.radius;

            // 中心間の距離が半径の合計以上なら衝突しない
            if (distanceSq >= radiusSum * radiusSum)
                continue;

            // 球体とカプセルの重なり量を求める
            float distance = sqrtf(distanceSq);
            float penetration = radiusSum - distance;

            // 複数のカプセルがある場合、最も深く重なった結果を残す
            if (penetration <= bestPenetration)
                continue;

            // 衝突した
            collisionInfo_.hit = true;

            // 球体の中心から最近接点へ向かう方向を衝突法線とする
            // 二点がほぼ同じ位置にある場合は、オブジェクトの位置関係から方向を求める
            collisionInfo_.normal = distance > 0.0001f ?
                VScale(delta, 1.0f / distance) :
                Math::SafeNormalize(VSub(cap->GetOwner()->GetComponent<Transform>()->pos_, transform_->pos_));

            collisionInfo_.penetration = penetration;

            // 最近接点と球体の中心の中間を衝突位置とする
            collisionInfo_.point = VScale(VAdd(closest, centerA), 0.5f);

            bestPenetration = penetration;
        }

        return collisionInfo_.hit;
    }

    // 対応していない種類のコライダーとは判定しない
    return false;
}

#ifdef _DEBUG

void SphereCollider::DebugDraw() const
{
    if (!transform_)
        return;

    // 球の中心と半径を取得する
    VECTOR center = GetCenter();
    float radius = GetRadius();

    // 緑色のワイヤーフレームで表示する
    DrawSphere3D(
        center,
        radius,
        16,
        GetColor(0, 255, 0),
        GetColor(0, 255, 0),
        FALSE
    );
}

#endif // _DEBUG

