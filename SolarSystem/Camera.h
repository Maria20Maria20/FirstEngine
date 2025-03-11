#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


class Camera
{
public:
    Camera();
    ~Camera();

    void SetPosition(Vector3 position);
    void SetTarget(Vector3 target);
    void SetUp(Vector3 up);

    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);
    void SetNearZ(float nearZ);
    void SetFarZ(float farZ);

    void Update(float deltaTime, const Matrix targetTransform);

    XMMATRIX GetViewMatrix() const;
    XMMATRIX GetProjectionMatrix() const;

    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void MoveUp(float speed);
    void MoveDown(float speed);

    void RotateYaw(float angle);
    void RotatePitch(float angle);

    void SwitchToOrbitalMode(Vector3 orbitalTarget);
    void SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis);
    void SwitchToOrbitalMode(Vector3 orbitalTarget, Vector3 spinAxis, float referenceLen);

    void SwitchToFPSMode();

    void SwitchProjection();

private:
    Vector3 position;
    Vector3 target;
    Vector3 up;

    float fov;
    float aspectRatio;
    float nearZ;
    float farZ;

    float orthZ;

    bool isOrbitalMode;
    Vector3 orbitalTarget;
    float defaultOrbitalDistance;
    float orbitalDistance;
    float orbitalYaw;
    float orbitalPitch;

    float orbitalAngleSpeed;
    Vector3 spinAxis;

    bool isPerspective = true;
};

#endif // CAMERA_H