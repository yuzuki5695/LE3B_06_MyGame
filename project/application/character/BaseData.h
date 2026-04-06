#pragma once
#include <Transform.h>
#include <string>

namespace MyGame {
	// キャラクターデータ構造体
    struct BaseData {
		// ファイル名
        std::string name;
		// Transform（位置、回転、スケール）
        MyEngine::Transform transform;
    };
}