#include "camera.h"
#include <math.h>

const float PI = 3.14159265359;

void Camera::updateYaw(float amount)
{
    yaw += amount;

    if (yaw < -PI)
    {
        yaw += 2.0 * PI;
    }
    if (yaw > PI)
    {
        yaw -= 2.0 * PI;
    }
}

void Camera::updatePitch(float amount)
{
    pitch += amount;

    if (pitch < -PI/2.0)
    {
        pitch = -PI/2.0;
    }
    if (pitch > PI/2.0)
    {
        pitch = PI/2.0;
    }
}

float Camera::getYaw()
{
    return yaw;
}

float Camera::getPitch()
{
    return pitch;
}

void Camera::strafe(float amount)
{
    x += amount * cos(yaw);
    z += amount * sin(yaw);
}

void Camera::move(float amount)
{
    x += amount * sin(yaw) * cos(pitch);
    y -= amount * sin(pitch);
    z -= amount * cos(yaw) * cos(pitch);
}
