#include "CapsuleCollider.h"
#include "SphereCollider.h"

#include "../../Object/Object.h"
#include "../../Math/Math.h"

#include <algorithm>

bool CapsuleCollider::CheckCollision(Collider* other)
{
    CollisionInfo info;
    return GetCollisionInfo(other, info);
}

bool CapsuleCollider::GetCollisionInfo(Collider* other, CollisionInfo& info)
{
    // 前回の判定結果が残らないように初期化する
    info = CollisionInfo{};

    // 判定に必要な情報がなければ処理しない
    if (!transform_ || !other)
        return false;

    // カプセル同士の衝突判定
    if (auto cap = dynamic_cast<CapsuleCollider*>(other))
    {
        if (!cap->transform_)
            return false;

        // 複数のカプセルがある場合に、最も深く重なった組み合わせを記録する
        float bestPenetration = 0.0f;

        // 自分が持つ全てのカプセルを判定
        for (const auto& a : capsules_)
        {
            // オフセットにオブジェクトの座標を加え、ワールド座標に変換する
            VECTOR aStart = VAdd(transform_->pos_, a.startOffset);
            VECTOR aEnd = VAdd(transform_->pos_, a.endOffset);

            // 相手側が持つ全てのカプセルを判定
            for (const auto& b : cap->capsules_)
            {
                VECTOR bStart = VAdd(cap->transform_->pos_, b.startOffset);
                VECTOR bEnd = VAdd(cap->transform_->pos_, b.endOffset);

                VECTOR pointA;
                VECTOR pointB;

                // 2本の線分のうち、最も近い点をそれぞれ求める
                Math::ClosestPointsSegmentSegment(
                    aStart, aEnd,
                    bStart, bEnd,
                    pointA, pointB);

                // 最も近い点同士の距離を求める
                VECTOR delta = VSub(pointB, pointA);
                float distanceSq = Math::LengthSq(delta);
                float radiusSum = a.radius + b.radius;

                // 半径の合計より離れていれば、衝突していない
                if (distanceSq >= radiusSum * radiusSum)
                    continue;

                // 重なり量を求める
                float distance = sqrtf(distanceSq);
                float penetration = radiusSum - distance;

                // 既にある重なりより浅ければ更新しない
                if (penetration <= bestPenetration)
                    continue;

                // 衝突している
                info.hit = true;

                // pointAからpointBへ向かう方向を衝突法線として扱う
                // 2点がほぼ同じ位置の場合は、オブジェクトの位置関係から方向を求める
                info.normal = distance > 0.0001f ? 
                    VScale(delta, 1.0f / distance) :
                    Math::SafeNormalize(VSub(cap->transform_->pos_, transform_->pos_));

                info.penetration = penetration;

                // 二点の最近接点の中間を衝突位置とする
                info.point = VScale(VAdd(pointA, pointB), 0.5f);

                bestPenetration = penetration;
            }
        }

        return info.hit;
    }

    // カプセルと球の衝突判定
    if (auto sphere = dynamic_cast<SphereCollider*>(other))
    {
        // 相手の座標情報を取得できなければ処理しない
        if (!sphere->GetOwner() || !sphere->GetOwner()->GetComponent<Transform>())
            return false;

        VECTOR center = sphere->GetCenter();
        float bestPenetration = 0.0f;

        // 自分が持つ全てのカプセルを判定
        for (const auto& cap : capsules_)
        {
            VECTOR start = VAdd(transform_->pos_, cap.startOffset);
            VECTOR end = VAdd(transform_->pos_, cap.endOffset);

            // 球の中心からカプセルの線分上で最も近い点を求める
            VECTOR closest = Math::ClosestPointOnSegment(center, start, end);

            VECTOR delta = VSub(center, closest);
            float distanceSq = Math::LengthSq(delta);
            float radiusSum = cap.radius + sphere->GetRadius();

            // 半径の合計より離れていれば衝突していない
            if (distanceSq >= radiusSum * radiusSum)
                continue;

            // カプセルと球の重なり量を求める
            float distance = sqrtf(distanceSq);
            float penetration = radiusSum - distance;

            // 最も深くなっている組み合わせを残す
            if (penetration <= bestPenetration)
                continue;

            info.hit = true;

            // カプセル側の最近接点から球の中心へ向かう方向を求める
            // 中心と最近接点が重なった場合は、オブジェクトの位置関係を使う
            info.normal = distance > 0.0001f ?
                VScale(delta, 1.0f / distance) :
                Math::SafeNormalize(VSub(sphere->GetCenter(), transform_->pos_));

            info.penetration = penetration;

            // 最近接点と球の中心の中間を衝突位置とする
            info.point = VScale(VAdd(closest, center), 0.5f);

            bestPenetration = penetration;
        }

        return info.hit;
    }

    // 対応していない種類のコライダーとは衝突判定を行わない
    return false;
}

#ifdef _DEBUG
void CapsuleCollider::DebugDraw() const
{
    if (!transform_)
        return;

    // 1つのコライダーに複数のカプセルが設定されている場合にも対応する
    for (const auto& capsule : capsules_)
    {
        // オブジェクトの位置とオフセットから、カプセルの両端を求める
        VECTOR start = VAdd(transform_->pos_, capsule.startOffset);
        VECTOR end = VAdd(transform_->pos_, capsule.endOffset);

        // 緑色のワイヤーフレームで表示する
        DrawCapsule3D(
            start,
            end,
            capsule.radius,
            16,
            GetColor(0, 255, 0),
            GetColor(0, 255, 0),
            FALSE
        );
    }
}
#endif // _DEBUG
