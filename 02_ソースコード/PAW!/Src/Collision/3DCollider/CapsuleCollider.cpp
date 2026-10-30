#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "CylinderCollider.h"

#include "../../Object/Object.h"
#include "../../Math/Math.h"

#include <algorithm>
#include <cmath>

namespace
{
    // 円柱(Y軸平行・中心c・半径r・半高h)上で、点pに最も近い点を求める
    VECTOR ClosestPointOnCylinder(const VECTOR& p, const VECTOR& c, float r, float halfH)
    {
        // 水平方向は円内に収める
        float dx = p.x - c.x;
        float dz = p.z - c.z;
        float d = sqrtf(dx * dx + dz * dz);
        if (d > r)
        {
            float s = r / d;
            dx *= s;
            dz *= s;
        }

        // 高さ方向は上下の範囲に収める
        float y = fmaxf(c.y - halfH, fminf(p.y, c.y + halfH));

        return VGet(c.x + dx, y, c.z + dz);
    }
}

bool CapsuleCollider::CheckCollision(Collider* other)
{
    // 前回の判定結果が残らないように初期化する
    collisionInfo_ = CollisionInfo{};

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
                collisionInfo_.hit = true;

                // pointAからpointBへ向かう方向を衝突法線として扱う
                // 2点がほぼ同じ位置の場合は、オブジェクトの位置関係から方向を求める
                collisionInfo_.normal = distance > 0.0001f ?
                    VScale(delta, 1.0f / distance) :
                    Math::SafeNormalize(VSub(cap->transform_->pos_, transform_->pos_));

                collisionInfo_.penetration = penetration;

                // 二点の最近接点の中間を衝突位置とする
                collisionInfo_.point = VScale(VAdd(pointA, pointB), 0.5f);

                bestPenetration = penetration;
            }
        }

        return collisionInfo_.hit;
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

            collisionInfo_.hit = true;

            // カプセル側の最近接点から球の中心へ向かう方向を求める
            // 中心と最近接点が重なった場合は、オブジェクトの位置関係を使う
            collisionInfo_.normal = distance > 0.0001f ?
                VScale(delta, 1.0f / distance) :
                Math::SafeNormalize(VSub(sphere->GetCenter(), transform_->pos_));

            collisionInfo_.penetration = penetration;

            // 最近接点と球の中心の中間を衝突位置とする
            collisionInfo_.point = VScale(VAdd(closest, center), 0.5f);

            bestPenetration = penetration;
        }

        return collisionInfo_.hit;
    }

    // カプセルと円柱の衝突判定
    if (auto cyl = dynamic_cast<CylinderCollider*>(other))
    {
        // 相手の座標情報を取得できなければ処理しない
        if (!cyl->GetOwner() || !cyl->GetOwner()->GetComponent<Transform>())
            return false;

        VECTOR center = cyl->GetCenter();
        float halfH = cyl->GetHeight() * 0.5f;
        float cylRadius = cyl->GetRadius();
        float bestPenetration = 0.0f;

        // 自分が持つ全てのカプセルを判定
        for (const auto& cap : capsules_)
        {
            VECTOR start = VAdd(transform_->pos_, cap.startOffset);
            VECTOR end = VAdd(transform_->pos_, cap.endOffset);

            // 円柱と線分の最近接点を、交互に射影して求める
            // (円柱も線分も凸なので、繰り返すと最近接点に収束する)
            VECTOR p = Math::ClosestPointOnSegment(center, start, end);
            for (int i = 0; i < 4; ++i)
            {
                VECTOR q = ClosestPointOnCylinder(p, center, cylRadius, halfH);
                p = Math::ClosestPointOnSegment(q, start, end);
            }

            // p:カプセル線分上の最近接点 / q:円柱上の最近接点
            VECTOR q = ClosestPointOnCylinder(p, center, cylRadius, halfH);

            VECTOR delta = VSub(q, p);
            float distanceSq = Math::LengthSq(delta);

            VECTOR normal;
            float penetration;

            if (distanceSq > 0.000001f)
            {
                // 線分が円柱の外側にある場合
                float distance = sqrtf(distanceSq);

                // 半径以上離れていれば衝突しない
                if (distance >= cap.radius)
                    continue;

                penetration = cap.radius - distance;

                // カプセル側から円柱側へ向かう方向を衝突法線とする
                normal = VScale(delta, 1.0f / distance);
            }
            else
            {
                // 線分が円柱の内部に入り込んでいる場合
                // 最も近い面から外へ押し出す
                float dx = p.x - center.x;
                float dz = p.z - center.z;
                float distH = sqrtf(dx * dx + dz * dz);

                float penSide = cylRadius - distH;
                float penTop = (center.y + halfH) - p.y;
                float penBottom = p.y - (center.y - halfH);

                float minPen;
                VECTOR outward;   // 円柱から外へ向かう方向
                if (penSide <= penTop && penSide <= penBottom)
                {
                    minPen = penSide;
                    outward = distH > 0.0001f ?
                        VGet(dx / distH, 0.0f, dz / distH) :
                        VGet(1.0f, 0.0f, 0.0f);
                }
                else if (penTop <= penBottom)
                {
                    minPen = penTop;
                    outward = VGet(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    minPen = penBottom;
                    outward = VGet(0.0f, -1.0f, 0.0f);
                }

                penetration = minPen + cap.radius;

                // 法線はカプセル側から円柱側へ向けるので、外向きの逆にする
                normal = VScale(outward, -1.0f);

                // 円柱表面上の点を求め直す
                q = VSub(p, VScale(outward, minPen));
            }

            // 複数のカプセルがある場合、最も深く重なった結果を残す
            if (penetration <= bestPenetration)
                continue;

            collisionInfo_.hit = true;
            collisionInfo_.normal = normal;
            collisionInfo_.penetration = penetration;

            // カプセル表面と円柱表面の中間を衝突位置とする
            VECTOR capSurface = VAdd(p, VScale(normal, cap.radius));
            collisionInfo_.point = VScale(VAdd(q, capSurface), 0.5f);

            bestPenetration = penetration;
        }

        return collisionInfo_.hit;
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
