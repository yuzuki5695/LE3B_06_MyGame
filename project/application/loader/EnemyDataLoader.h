#pragma once
#include <string>
#include <PlayerData.h>
#include <json.hpp>

class PlayerDataLoader {
public:// メンバ関数			
    static PlayerData Load(const std::string& path);
    static Transform LoadTransform(const nlohmann::json& transform);
    static  PlayerMoveData LoadMove(const nlohmann::json& move);
    static  PlayerReticleData LoadReticle(const nlohmann::json& reticle);
    static PlayerWeaponData LoadWeapon(const nlohmann::json& weapon);
    static PlayerDeathData LoadDeath(const nlohmann::json& death);
private: // メンバ変数
    static const std::string kDefaultBaseDirectory;
    static const std::string kExtension;
};