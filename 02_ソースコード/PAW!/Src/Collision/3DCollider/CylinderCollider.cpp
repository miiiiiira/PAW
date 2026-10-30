#include "CylinderCollider.h"
#include "CapsuleCollider.h"

#include "../../Object/Object.h"
#include "../../Math/Math.h"
#include "../../Math/MatrixUtility.h"

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

VECTOR CylinderCollider::GetCenter() const
{
    // 回転行列
    VECTOR vec = { transform_->angle_.x,transform_->angle_.y,0.0f };
    MATRIX matRot = Matrix::GetMatrixRotateXYZ(vec);

    // 指定の座標に回転を適用する
    VECTOR localPosRot = VTransform(centerOffset, matRot);

    return VAdd(transform_->pos_, localPosRot);
}

bool CylinderCollider::CheckCollision(Collider* other)
{
    // 前回の判定結果が残らないように初期化する
    collisionInfo_ = CollisionInfo{};

    // 判定に必要な情報がなければ処理しない
    if (!transform_ || !other)
        return false;

    // 自身の円柱の中心位置と半高を取得する
    VECTOR centerA = GetCenter();
    float halfA = height_ * 0.5f;

    // 円柱同士の衝突判定
    if (auto cyl = dynamic_cast<CylinderCollider*>(other))
    {
        // 相手の座標情報を取得できなければ判定しない
        if (!cyl->GetOwner() || !cyl->GetOwner()->GetComponent<Transform>())
            return false;

        VECTOR centerB = cyl->GetCenter();
        float halfB = cyl->GetHeight() * 0.5f;

        // 水平方向(XZ平面)の円同士の距離
        float dx = centerB.x - centerA.x;
        float dz = centerB.z - centerA.z;
        float distH = sqrtf(dx * dx + dz * dz);
        float radiusSum = radius_ + cyl->GetRadius();

        // 水平方向で離れていれば接触していない
        if (distH >= radiusSum)
            return false;

        // 垂直方向の範囲の重なり
        float dy = centerB.y - centerA.y;
        float halfSum = halfA + halfB;

        // 垂直方向で離れていれば接触していない
        if (fabsf(dy) >= halfSum)
            return false;

        float penH = radiusSum - distH;       // 水平方向のめり込み量
        float penV = halfSum - fabsf(dy);     // 垂直方向のめり込み量

        collisionInfo_.hit = true;

        // めり込みが小さい方向に押し出す
        if (penH < penV)
        {
            // 側面同士の衝突:水平方向に押し出す
            // 中心がほぼ同じ位置の場合は、仮の方向を設定する
            collisionInfo_.normal = distH > 0.0001f ?
                VGet(dx / distH, 0.0f, dz / distH) :
                VGet(1.0f, 0.0f, 0.0f);

            collisionInfo_.penetration = penH;

            // 重なっている高さ範囲の中間を求める
            float overlapTop = fminf(centerA.y + halfA, centerB.y + halfB);
            float overlapBottom = fmaxf(centerA.y - halfA, centerB.y - halfB);
            float midY = (overlapTop + overlapBottom) * 0.5f;

            VECTOR p = VAdd(centerA, VScale(collisionInfo_.normal, radius_ - penH * 0.5f));
            p.y = midY;
            collisionInfo_.point = p;
        }
        else
        {
            // 上下面の衝突:垂直方向に押し出す
            float sign = dy >= 0.0f ? 1.0f : -1.0f;

            collisionInfo_.normal = VGet(0.0f, sign, 0.0f);
            collisionInfo_.penetration = penV;

            // 自身の上(下)面付近、水平位置は両中心の中間とする
            collisionInfo_.point = VGet(
                (centerA.x + centerB.x) * 0.5f,
                centerA.y + sign * (halfA - penV * 0.5f),
                (centerA.z + centerB.z) * 0.5f);
        }

        return true;
    }

    // 円柱とカプセルの衝突判定
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

            // 円柱と線分の最近接点を、交互に射影して求める
            // (円柱も線分も凸なので、繰り返すと最近接点に収束する)
            VECTOR p = Math::ClosestPointOnSegment(centerA, start, end);
            for (int i = 0; i < 4; ++i)
            {
                VECTOR q = ClosestPointOnCylinder(p, centerA, radius_, halfA);
                p = Math::ClosestPointOnSegment(q, start, end);
            }

            // p:カプセル線分上の最近接点 / q:円柱上の最近接点
            VECTOR q = ClosestPointOnCylinder(p, centerA, radius_, halfA);

            VECTOR delta = VSub(p, q);
            float distanceSq = Math::LengthSq(delta);

            VECTOR normal;
            float penetration;

            if (distanceSq > 0.000001f)
            {
                // 線分が円柱の外側にある場合
                float distance = sqrtf(distanceSq);

                // 半径以上離れていれば衝突しない
                if (distance >= capsule.radius)
                    continue;

                penetration = capsule.radius - distance;

                // 円柱側からカプセル側へ向かう方向を衝突法線とする
                normal = VScale(delta, 1.0f / distance);
            }
            else
            {
                // 線分が円柱の内部に入り込んでいる場合
                // 最も近い面から外へ押し出す
                float dx = p.x - centerA.x;
                float dz = p.z - centerA.z;
                float distH = sqrtf(dx * dx + dz * dz);

                float penSide = radius_ - distH;
                float penTop = (centerA.y + halfA) - p.y;
                float penBottom = p.y - (centerA.y - halfA);

                float minPen;
                if (penSide <= penTop && penSide <= penBottom)
                {
                    minPen = penSide;
                    normal = distH > 0.0001f ?
                        VGet(dx / distH, 0.0f, dz / distH) :
                        VGet(1.0f, 0.0f, 0.0f);
                }
                else if (penTop <= penBottom)
                {
                    minPen = penTop;
                    normal = VGet(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    minPen = penBottom;
                    normal = VGet(0.0f, -1.0f, 0.0f);
                }

                penetration = minPen + capsule.radius;

                // 円柱表面上の点を求め直す
                q = VSub(p, VScale(normal, minPen));
            }

            // 複数のカプセルがある場合、最も深く重なった結果を残す
            if (penetration <= bestPenetration)
                continue;

            // 衝突あり
            collisionInfo_.hit = true;
            collisionInfo_.normal = normal;
            collisionInfo_.penetration = penetration;

            // 円柱表面とカプセル表面の中間を衝突位置とする
            VECTOR capSurface = VSub(p, VScale(normal, capsule.radius));
            collisionInfo_.point = VScale(VAdd(q, capSurface), 0.5f);

            bestPenetration = penetration;
        }

        return collisionInfo_.hit;
    }

    // 対応していない種類のコライダーとは判定しない
    return false;
}

#ifdef _DEBUG

void CylinderCollider::DebugDraw() const
{
    if (!transform_)
        return;

    VECTOR center = GetCenter();
    float halfH = height_ * 0.5f;

    // 上下の円を緑色のラインで描画し、側面を4本の線でつなぐ
    const int div = 24;
    const float pi2 = 6.2831853f;
    unsigned int color = GetColor(0, 255, 0);

    for (int i = 0; i < div; ++i)
    {
        float a0 = pi2 * i / div;
        float a1 = pi2 * (i + 1) / div;

        VECTOR b0 = VGet(center.x + cosf(a0) * radius_, center.y - halfH, center.z + sinf(a0) * radius_);
        VECTOR b1 = VGet(center.x + cosf(a1) * radius_, center.y - halfH, center.z + sinf(a1) * radius_);
        VECTOR t0 = VGet(b0.x, center.y + halfH, b0.z);
        VECTOR t1 = VGet(b1.x, center.y + halfH, b1.z);

        DrawLine3D(b0, b1, color);
        DrawLine3D(t0, t1, color);

        if (i % (div / 4) == 0)
            DrawLine3D(b0, t0, color);
    }
}

#endif // _DEBUG
