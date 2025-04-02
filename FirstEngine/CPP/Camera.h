#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


class Camera
{
public:
    enum class CAMERA_MODE
    {
        FPS, ORBITAL, FOLLOW
    };
    Camera();
    Camera(float aspectRatio);
    ~Camera();

    void SetPosition(Vector3 position);
    void SetTarget(Vector3 target);
    void SetUp(Vector3 up);

    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);
    void SetNearZ(float nearZ);
    void SetFarZ(float farZ);

    void Update(float deltaTime, const Matrix targetTransform);
    void Update(float deltaTime, const Matrix targetTransform, Vector3 direction);
    void Update(float deltaTime, const Matrix targetTransform, Vector3 direction, float referenceLen);


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

    void SwitchToFollowMode(Vector3 followTarget, Vector3 direction, float referenceLen);

    void SwitchProjection();

    Vector3 GetPosition();

private:
    Vector3 position;
    Vector3 target;
    Vector3 up;

    float fov;
    float aspectRatio;
    float nearZ;
    float farZ;

    float orthZ;

    float referenceLen;

    CAMERA_MODE cameraMode = CAMERA_MODE::FPS;

    Vector3 orbitalTarget;
    float minOrbitalDistance;
    float orbitalDistance;
    float orbitalYaw;
    float orbitalPitch;
    float orbitalAngleSpeed;

    Vector3 spinAxis;

    bool isPerspective = true;

    float followPitch;
};

#endif // CAMERA_H