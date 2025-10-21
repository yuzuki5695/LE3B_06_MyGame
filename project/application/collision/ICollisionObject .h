#pragma once
#include <OBB.h>

class ICollisionObject {
public:
    virtual ~ICollisionObject() = default;
    virtual OBB GetOBB() const = 0;
};