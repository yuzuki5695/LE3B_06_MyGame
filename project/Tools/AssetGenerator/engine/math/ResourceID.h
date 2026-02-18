#pragma once
#include <string>

enum class ResourceType {
    Texture,
    Model,
    Audio
};

struct ResourceId {
    ResourceType type;   // Texture / Model / Audio
    std::string  name;   // enemybullet / alarm01
    std::string  path;   // Bullet/EnemyBullet.obj
};
