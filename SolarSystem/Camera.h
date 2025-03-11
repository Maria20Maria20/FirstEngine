#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <SimpleMath.h>
#include <iostream>

class Camera
{
public:
    Camera();
    ~Camera();

    void SetPosition(DirectX::SimpleMath::Vector3 position);
    void SetTarget(DirectX::SimpleMath::Vector3 target);
    void SetUp(DirectX::SimpleMath::Vector3 up);

    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);
    void SetNearZ(float nearZ);
    void SetFarZ(float farZ);

    void Update(float deltaTime, const DirectX::SimpleMath::Matrix targetTransform);

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void MoveUp(float speed);
    void MoveDown(float speed);

    void RotateYaw(float angle);
    void RotatePitch(float angle);

    void SwitchToOrbitalMode(DirectX::SimpleMath::Vector3 orbitalTarget);
    void SwitchToOrbitalMode(DirectX::SimpleMath::Vector3 orbitalTarget, DirectX::SimpleMath::Vector3 rotAxis);
    void SwitchToOrbitalMode(DirectX::SimpleMath::Vector3 orbitalTarget, DirectX::SimpleMath::Vector3 rotAxis, float orbitalDistance);

    void SwitchToFPSMode();

    void SwitchProjection();

private:
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 target;
    DirectX::SimpleMath::Vector3 up;

    float fov;
    float aspectRatio;
    float nearZ;
    float farZ;

    float orthZ;

    bool isOrbitalMode;
    DirectX::SimpleMath::Vector3 orbitalTarget;
    float defaultOrbitalDistance;
    float orbitalDistance;
    float orbitalYaw;
    float orbitalPitch;

    float orbitalAngleSpeed;
    DirectX::SimpleMath::Vector3 orbitalAxis;

    bool isPerspective = true;
};

#endif // CAMERA_H