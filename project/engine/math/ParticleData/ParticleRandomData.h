#pragma once
#include <Velocity.h>
#include <Vector4.h>

struct ParticleRandomData {
    Vector3 offset;
    Vector3 rotation;
    Vector3 scale;
    Velocity velocity;
    float lifetime;
    Vector4 color;
    Vector3 rotationSpeed;
    Vector3 scaleSpeed;
};