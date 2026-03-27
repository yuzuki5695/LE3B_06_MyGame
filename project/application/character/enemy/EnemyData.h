#pragma once
#include <Transform.h>
#include <string>

enum class EnemyMoveType {
    Stationary, // その場に固定（マップ配置）
    RailFollow  // レールカメラに同期（画面内に居座る）
};

struct EnemyData {
    std::string fileName; // 名前
    Transform transform;  // 
};