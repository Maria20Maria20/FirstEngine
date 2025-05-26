#ifndef FLOOR_HIEGHT_FUNC
#define FLOOR_HIEGHT_FUNC

float FloorHeightFunc(float3 pos)
{
    return (sin(pos.x * 0.5) * cos(pos.z * 0.5) + 0 * sin(pos.x * 10.0f) * 0.2) * 0.0;
}

#endif
