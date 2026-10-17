#include "Math.h"

#include <sstream>
#include <cmath>
#include <algorithm>

namespace Math
{
    int Round(float v) { return static_cast<int>(roundf(v)); }

    std::vector<std::string> Split(const std::string& line, char delimiter)
    {
        std::istringstream stream(line);
        std::string field;
        std::vector<std::string> result;
        while (getline(stream, field, delimiter))
            result.push_back(field);
        return result;
    }

    double Rad2Deg(double rad) { return rad * (180.0 / DX_PI); }
    float  Rad2Deg(float rad) { return rad * RAD2DEG; }
    double Deg2Rad(double deg) { return deg * (DX_PI / 180.0); }
    float  Deg2Rad(float deg) { return deg * DEG2RAD; }

    double DegIn360(double deg)
    {
        deg = fmod(deg, 360.0);
        if (deg < 0.0) deg += 360.0;
        return deg;
    }

    double RadIn2PI(double rad)
    {
        rad = fmod(rad, DX_TWO_PI);
        if (rad < 0.0) rad += DX_TWO_PI;
        return rad;
    }

    float NormalizeAngle(float rad)
    {
        while (rad > DX_PI_F) rad -= DX_TWO_PI_F;
        while (rad < -DX_PI_F) rad += DX_TWO_PI_F;
        return rad;
    }

    int DirNearAroundRad(float from, float to)
    {
        float diff = to - from;
        if (diff >= 0.0f)
            return (diff > DX_PI_F) ? -1 : 1;
        else
            return (diff < -DX_PI_F) ? 1 : -1;
    }

    int DirNearAroundDeg(float from, float to)
    {
        float diff = to - from;
        if (diff >= 0.0f)
            return (diff > 180.0f) ? -1 : 1;
        else
            return (diff < -180.0f) ? 1 : -1;
    }

    int Lerp(int a, int b, float t)
    {
        if (t >= 1.0f) return b;
        return a + Round((b - a) * t);
    }

    float Lerp(float a, float b, float t)
    {
        if (t >= 1.0f) return b;
        return a + (b - a) * t;
    }

    double Lerp(double a, double b, double t)
    {
        if (t >= 1.0) return b;
        return a + (b - a) * t;
    }

    Vector2 Lerp(const Vector2& a, const Vector2& b, float t)
    {
        return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
    }

    VECTOR Lerp(const VECTOR& a, const VECTOR& b, float t)
    {
        return VGet(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }

    double LerpDeg(double a, double b, double t)
    {
        double diff = b - a;
        if (diff > 180.0)  b -= 360.0;
        if (diff < -180.0) b += 360.0;
        return Lerp(a, b, t);
    }

    float LerpAngle(float from, float to, float t)
    {
        float diff = NormalizeAngle(to - from);
        return from + diff * t;
    }

    bool SmoothAngle(float& angle, float targetAngle, float& velocity, float smoothTime )
    {
        if (smoothTime < 0.0001f) smoothTime = 0.0001f; 

        const float dt = 1.0f / 60.0f; // 60fps固定

        float diff = NormalizeAngle(targetAngle - angle);

        float omega = 2.0f / smoothTime;
        float x = omega * dt;

        float decay = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        float change = -diff;                           
        float temp = (velocity + omega * change) * dt;
        velocity = (velocity - omega * temp) * decay;
        float output = (change + temp) * decay;         

        angle = NormalizeAngle(targetAngle + output);

        // 到達判定：角度差と速度の両方が小さい
        const float angleEps = 0.02f;  // 約1.1度
        const float velEps = 0.02f;  // 1秒あたりのラジアン（かなりゆっくり）
        float remain = NormalizeAngle(targetAngle - angle);

        return (fabsf(remain) < angleEps) && (fabsf(velocity) < velEps);
    }

    COLOR_F Lerp(const COLOR_F& a, const COLOR_F& b, float t)
    {
        return {
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
        };
    }

    Vector2 Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
    {
        return Lerp(Lerp(p1, p2, t), Lerp(p2, p3, t), t);
    }

    VECTOR Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t)
    {
        return Lerp(Lerp(p1, p2, t), Lerp(p2, p3, t), t);
    }

    double Magnitude(const Vector2& v) { return sqrt(v.x * v.x + v.y * v.y); }
    double Magnitude(const VECTOR& v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
    float  MagnitudeF(const VECTOR& v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

    float  SqrMagnitude(const Vector2& v) { return v.x * v.x + v.y * v.y; }
    float  SqrMagnitudeF(const VECTOR& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
    double SqrMagnitude(const VECTOR& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

    double SqrMagnitude(const VECTOR& a, const VECTOR& b)
    {
        return pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2);
    }

    double Distance(const Vector2& a, const Vector2& b)
    {
        return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
    }

    double Distance(const VECTOR& a, const VECTOR& b)
    {
        return sqrt(SqrMagnitude(a, b));
    }

    bool Equals(const VECTOR& a, const VECTOR& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    bool EqualsVZero(const VECTOR& v1)
    {
        const VECTOR& v2 = VECTOR_ZERO;
        if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
        {
            return true;
        }
        return false;
    }

    bool EqualsZero(const VECTOR& v)
    {
        return Equals(v, VECTOR_ZERO);
    }

    VECTOR Normalize(const Vector2& v)
    {
        float len = static_cast<float>(Magnitude(v));
        if (len < kEpsilonNormalSqrt) return VECTOR_ZERO;
        return VGet(v.x / len, v.y / len, 0.0f);
    }

    VECTOR Normalize(const VECTOR& v)
    {
        if (EqualsZero(v)) return v;
        return VNorm(v);
    }

    double AngleDeg(const VECTOR& from, const VECTOR& to)
    {
        double d = sqrt(SqrMagnitude(from) * SqrMagnitude(to));
        if (d < kEpsilonNormalSqrt) return 0.0;

        double dot = VDot(from, to) / d;
        dot = std::clamp(dot, -1.0, 1.0);
        return acos(dot) * (180.0 / DX_PI);
    }

    float LengthSq(const VECTOR& v)
    {
        return VDot(v, v);
    }

    VECTOR SafeNormalize(const VECTOR& v)
    {
        float lenSq = LengthSq(v);

        if (lenSq < 0.000001f)
        {
            return VGet(1.0f, 0.0f, 0.0f);
        }

        return VScale(v, 1.0f / std::sqrt(lenSq));
    }

    VECTOR ClosestPointOnSegment(const VECTOR& p, const VECTOR& a, const VECTOR& b)
    {
        VECTOR ab = VSub(b, a);
        float lengthSq = LengthSq(ab);

        if (lengthSq < 0.000001f)
        {
            return a;
        }

        float t = VDot(VSub(p, a), ab) / lengthSq;
        t = max(0.0f, min(1.0f, t));

        return VAdd(a, VScale(ab, t));
    }

    void ClosestPointsSegmentSegment(const VECTOR& p1, const VECTOR& q1, const VECTOR& p2, const VECTOR& q2, VECTOR& c1, VECTOR& c2)
    {
        const float eps = 0.000001f;

        VECTOR d1 = VSub(q1, p1);
        VECTOR d2 = VSub(q2, p2);
        VECTOR r = VSub(p1, p2);

        float a = VDot(d1, d1);
        float e = VDot(d2, d2);
        float f = VDot(d2, r);

        float s = 0.0f;
        float t = 0.0f;

        if (a <= eps && e <= eps)
        {
            c1 = p1;
            c2 = p2;
            return;
        }

        if (a <= eps)
        {
            t = max(0.0f, min(1.0f, f / e));
        }
        else
        {
            float c = VDot(d1, r);

            if (e <= eps)
            {
                s = max(0.0f, min(1.0f, -c / a));
            }
            else
            {
                float b = VDot(d1, d2);
                float denom = a * e - b * b;

                if (std::fabs(denom) > eps)
                {
                    s = max(0.0f, min(1.0f, (b * f - c * e) / denom));
                }

                t = (b * s + f) / e;

                if (t < 0.0f)
                {
                    t = 0.0f;
                    s = max(0.0f, min(1.0f, -c / a));
                }
                else if (t > 1.0f)
                {
                    t = 1.0f;
                    s = max(0.0f, min(1.0f, (b - c) / a));
                }
            }
        }

        c1 = VAdd(p1, VScale(d1, s));
        c2 = VAdd(p2, VScale(d2, t));
    }
}
