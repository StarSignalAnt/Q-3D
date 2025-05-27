#pragma once
#include "BasicMath.hpp"
using namespace Diligent;


// Function to convert radians to degrees
float RadiansToDegrees(float radians)
{
    return radians * (180.0f / 3.14159265358979323846f);
}

// Function to convert degrees to radians
float DegreesToRadians(float degrees)
{
    return degrees * (3.14159265358979323846f / 180.0f);
}

// Custom Quaternion class
class Quat
{
public:
    float x, y, z, w;

    Quat(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float w_ = 1.0f) : x(x_), y(y_), z(z_), w(w_) {}

    // Create quaternion from yaw, pitch, roll angles
    static Quat CreateFromYawPitchRoll(float yaw, float pitch, float roll)
    {
        yaw *= 0.5f;
        pitch *= 0.5f;
        roll *= 0.5f;

        float cy = std::cos(yaw);
        float sy = std::sin(yaw);
        float cp = std::cos(pitch);
        float sp = std::sin(pitch);
        float cr = std::cos(roll);
        float sr = std::sin(roll);

        return Quat(
            cy * sp * cr + sy * cp * sr,
            sy * cp * cr - cy * sp * sr,
            cy * cp * sr - sy * sp * cr,
            cy * cp * cr + sy * sp * sr);
    }

    // Convert quaternion to rotation matrix
    float4x4 ToRotationMatrix() const
    {
        float4x4 m;

        float xx = x * x;
        float yy = y * y;
        float zz = z * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;

        m.m00 = 1.0f - 2.0f * (yy + zz);
        m.m01 = 2.0f * (xy + wz);
        m.m02 = 2.0f * (xz - wy);
        m.m03 = 0.0f;

        m.m10 = 2.0f * (xy - wz);
        m.m11 = 1.0f - 2.0f * (xx + zz);
        m.m12 = 2.0f * (yz + wx);
        m.m13 = 0.0f;

        m.m20 = 2.0f * (xz + wy);
        m.m21 = 2.0f * (yz - wx);
        m.m22 = 1.0f - 2.0f * (xx + yy);
        m.m23 = 0.0f;

        m.m30 = 0.0f;
        m.m31 = 0.0f;
        m.m32 = 0.0f;
        m.m33 = 1.0f;

        return m;
    }

    // Create quaternion from rotation matrix
    static Quat CreateFromRotationMatrix(const float4x4& m)
    {
        float trace = m.m00 + m.m11 + m.m22;
        if (trace > 0)
        {
            float s = 0.5f / std::sqrt(trace + 1.0f);
            return Quat(
                (m.m21 - m.m12) * s,
                (m.m02 - m.m20) * s,
                (m.m10 - m.m01) * s,
                0.25f / s);
        }
        else
        {
            if (m.m00 > m.m11 && m.m00 > m.m22)
            {
                float s = 2.0f * std::sqrt(1.0f + m.m00 - m.m11 - m.m22);
                return Quat(
                    0.25f * s,
                    (m.m01 + m.m10) / s,
                    (m.m02 + m.m20) / s,
                    (m.m21 - m.m12) / s);
            }
            else if (m.m11 > m.m22)
            {
                float s = 2.0f * std::sqrt(1.0f + m.m11 - m.m00 - m.m22);
                return Quat(
                    (m.m01 + m.m10) / s,
                    0.25f * s,
                    (m.m12 + m.m21) / s,
                    (m.m02 - m.m20) / s);
            }
            else
            {
                float s = 2.0f * std::sqrt(1.0f + m.m22 - m.m00 - m.m11);
                return Quat(
                    (m.m02 + m.m20) / s,
                    (m.m12 + m.m21) / s,
                    0.25f * s,
                    (m.m10 - m.m01) / s);
            }
        }
    }
};


