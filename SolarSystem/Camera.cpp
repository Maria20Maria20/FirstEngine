#include "Camera.h"
#include <iostream>

Camera::Camera()
    : position(0.0f, 0.0f, -5.0f), target(0.0f, 0.0f, 1.0f), up(0.0f, 1.0f, 0.0f),
    fov(XM_PIDIV4), aspectRatio(1.0f), nearZ(0.1f), farZ(1000.0f),
    orthZ(10.0f), isPerspective(true),
    isOrbitalMode(false), orbitalTarget(0.0f, 0.0f, 0.0f),
    orbitalDistance(5.0f), defaultOrbitalDistance(5.0f),
    orbitalPitch(0.0f), orbitalYaw(0.0f),
    spinAxis(0.0f, 1.0f, 0.0f), orbitalAngleSpeed(0.0f)
{

    //Matrix lookAt = XMMatrixLookAtLH(
    //    position, // Input camera position Q
    //    target, // Input target point T
    //    up); // Input world up vector j

    //Matrix proj = XMMatrixPerspectiveFovLH( // returns projection matrix
    //    fov, // vertical field of view angle in radians
    //    aspectRatio, // aspect ratio = width / height
    //    nearZ, // distance to near plane
    //    farZ); // distance to far plane

}

Camera::~Camera()
{
}

void Camera::SetPosition(Vector3 position)
{
    this->position = position;
}

void Camera::SetTarget(Vector3 target)
{
    this->target = target;
}

void Camera::SetUp(Vector3 up)
{
    this->up = up;
}

void Camera::SetFOV(float fov)
{
    this->fov = fov;
}

void Camera::SetAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;
}

void Camera::SetNearZ(float nearZ)
{
    this->nearZ = nearZ;
}

void Camera::SetFarZ(float farZ)
{
    this->farZ = farZ;
}

void Camera::Update(float deltaTime, const Matrix targetTransform = Matrix::Identity)
{
    if (isOrbitalMode)
    {
        //orbitalYaw += orbitalAngleSpeed * deltaTime;

        position.x = orbitalDistance * cosf(orbitalPitch) * cosf(orbitalYaw);
        position.y = orbitalDistance * sinf(orbitalPitch);
        position.z = orbitalDistance * cosf(orbitalPitch) * sinf(orbitalYaw);

        position = Vector3::Transform(position, targetTransform);

        orbitalTarget = Vector3::Transform(Vector3::Zero, targetTransform);
        target = orbitalTarget;

        up = Vector3::Transform(spinAxis, targetTransform) - orbitalTarget;

        std::cout << orbitalDistance << " :: ";
        std::cout << up.x << ", " << up.y << ", " << up.z << std::endl;

        // Обновление позиции камеры в орбитальном режиме

        /*position.x = orbitalTarget.x + orbitalDistance * cos(orbitalYaw) * cos(orbitalPitch);
        position.y = orbitalTarget.y + orbitalDistance * sin(orbitalPitch);
        position.z = orbitalTarget.z + orbitalDistance * sin(orbitalYaw) * cos(orbitalPitch);

        up = spinAxis;

        target = orbitalTarget;*/
        //Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle())
        //Vector3::Transform((), Matrix::CreateFromAxisAngle(spinAxis, orbitalAngleSpeed * deltaTime);

        //XMMatrixLookAtLH()

        //position = Vector3::Transform(orbitalTarget, Matrix::CreateFromYawPitchRoll(orbitalYaw, orbitalPitch) )
    }
}

XMMATRIX Camera::GetViewMatrix() const
{
    return XMMatrixLookAtLH(position, target, up);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
    if (isPerspective)
        return XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
    else
        return XMMatrixOrthographicLH(aspectRatio * 2.0f * tanf(0.5f * fov) * orthZ, 2.0f * tanf(0.5f * fov) * orthZ, nearZ, farZ);
}

void Camera::MoveForward(float speed)
{
    if (isOrbitalMode)
    {
        orbitalDistance = max(orbitalDistance + speed, 0.1f);
    }
    else
    {
        XMVECTOR forward = XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&position));
        forward = XMVector3Normalize(forward);
        position.x += speed * XMVectorGetX(forward);
        position.y += speed * XMVectorGetY(forward);
        position.z += speed * XMVectorGetZ(forward);
        target.x += speed * XMVectorGetX(forward);
        target.y += speed * XMVectorGetY(forward);
        target.z += speed * XMVectorGetZ(forward);
    }
    if (!isPerspective)
    {
        orthZ = max(orthZ + speed, nearZ * 1.1);
    }
}

void Camera::MoveBackward(float speed)
{
    MoveForward(-speed);
}

void Camera::MoveLeft(float speed)
{
    if (isOrbitalMode) {
        orbitalYaw -= speed;
    }
    else {
        XMVECTOR right = XMVector3Cross(
            XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&position)),
            XMLoadFloat3(&up)
        );
        right = XMVector3Normalize(right);
        position.x += speed * XMVectorGetX(right);
        position.y += speed * XMVectorGetY(right);
        position.z += speed * XMVectorGetZ(right);
        target.x += speed * XMVectorGetX(right);
        target.y += speed * XMVectorGetY(right);
        target.z += speed * XMVectorGetZ(right);
    }
}

void Camera::MoveRight(float speed)
{
    MoveLeft(-speed);
}

void Camera::MoveUp(float speed)
{
    if (isOrbitalMode) {
        orbitalPitch = max(-XM_PIDIV2 + 0.01f, min(orbitalPitch - speed, XM_PIDIV2 - 0.01f));
    }
    else {
        position.y += speed;
        target.y += speed;
    }
}

void Camera::MoveDown(float speed)
{
    MoveUp(-speed);
}

void Camera::RotateYaw(float angle)
{
    if (isOrbitalMode)
    {
        orbitalYaw += angle;
    }
    else
    {
        Vector3 look_dir = Vector3::Transform(target - position,
            Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(up, angle)));
        target = position + look_dir;
    }
}

void Camera::RotatePitch(float angle)
{
    if (isOrbitalMode)
    {
        orbitalPitch += angle;
    }
    else
    {
        Vector3 look_dir = target - position;
        Vector3 _axis = up.Cross(look_dir);
        look_dir = Vector3::Transform(look_dir,
            Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(_axis, angle)));
        target = position + look_dir;
    }
}

void Camera::SwitchToOrbitalMode(Vector3 orbitalTarget)
{
    SwitchToOrbitalMode(orbitalTarget, Vector3(0.0f, 1.0f, 0.0f), 1.0f);
}

void Camera::SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis)
{
    SwitchToOrbitalMode(orbitalTarget, spinAxis, 1.0f);
}
void Camera::SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis, float referenceLen)
{
    isOrbitalMode = true;
    orbitalAngleSpeed = 0.0f;
    this->orbitalDistance = 2.0f * referenceLen / tanf(fov * 0.5);
    orthZ = 2.0f * referenceLen / tanf(fov * 0.5);
    orbitalYaw = 0.0f;
    orbitalPitch = XM_PIDIV4;
    this->orbitalTarget = orbitalTarget;
    target = orbitalTarget;
    this->spinAxis = spinAxis;
    up = spinAxis;
    //orbitalDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&position), XMLoadFloat3(&target))));
}

void Camera::SwitchToFPSMode()
{
    isOrbitalMode = false;
}

void Camera::SwitchProjection() {
    isPerspective = !isPerspective;
    if (isOrbitalMode)
    {
        if (isPerspective)
            orbitalDistance = orthZ;
        else
            orthZ = orbitalDistance;
    }
    else {
        if (isPerspective)
            orbitalDistance = orthZ;
        else
            orthZ = (position - target).Length();
    }

}