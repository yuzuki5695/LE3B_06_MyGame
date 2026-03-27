#include "PlayerDataLoader.h"
#include <fstream>
#include <cassert>

using json = nlohmann::json;

const std::string PlayerDataLoader::kDefaultBaseDirectory = "Resources/Data/Player/";
const std::string PlayerDataLoader::kExtension = ".json";

PlayerData PlayerDataLoader::Load(const std::string& fileName) {
    // 連結してフルパスを得る
    const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

    PlayerData data{};

    // ファイルを開く
    std::ifstream file(fullpath);
    if (!file.is_open()) {
        throw std::runtime_error("JSONファイルを開けませんでした");
    }

    // JSONデータを読み込む
    json j;
    file >> j;

    // 必須チェック
    assert(j.contains("name"));
    assert(j.contains("transform"));
    assert(j.contains("move"));

    // データ取得
    data.fileName = j["name"].get<std::string>();
    data.transform = LoadTransform(j["transform"]);
    data.move = LoadMove(j["move"]);
    data.reticle = LoadReticle(j["reticle"]);
    data.weapon = LoadWeapon(j["weapon"]);
    data.reticle = LoadReticle(j["reticle"]);
    data.death = LoadDeath(j["death"]);
 
    return data;
}

PlayerMoveData PlayerDataLoader::LoadMove(const json& move) {
    PlayerMoveData result{};

    if (move.contains("speed")) result.speed = move["speed"];
    if (move.contains("maxX")) result.maxX = move["maxX"];
    if (move.contains("minY")) result.minY = move["minY"];
    if (move.contains("maxY")) result.maxY = move["maxY"];

    if (move.contains("tiltSpeed")) result.tiltSpeed = move["tiltSpeed"];
    if (move.contains("maxTilt")) result.maxTilt = move["maxTilt"];
    if (move.contains("returnSpeed")) result.returnSpeed = move["returnSpeed"];

    if (move.contains("dashDuration")) result.dashDuration = move["dashDuration"];
    if (move.contains("dashCoolDown")) result.dashCoolDown = move["dashCoolDown"];
    if (move.contains("dashSpeedMult")) result.dashSpeedMult = move["dashSpeedMult"];

    return result;
}

Transform PlayerDataLoader::LoadTransform(const json& transform) {
    Transform result{};

    // 安全チェック
    assert(transform.contains("translation"));
    assert(transform.contains("rotation"));
    assert(transform.contains("scaling"));

    // translation
    result.translate.x = (float)transform["translation"][0];
    result.translate.y = (float)transform["translation"][1];
    result.translate.z = (float)transform["translation"][2];

    // rotation
    result.rotate.x = -(float)transform["rotation"][0];
    result.rotate.y = -(float)transform["rotation"][2];
    result.rotate.z = -(float)transform["rotation"][1];

    // scale
    result.scale.x = (float)transform["scaling"][0];
    result.scale.y = (float)transform["scaling"][2];
    result.scale.z = (float)transform["scaling"][1];

    return result;
}

PlayerReticleData PlayerDataLoader::LoadReticle(const nlohmann::json& reticle) {
    PlayerReticleData result{};
    if (reticle.contains("speed")) result.speed = reticle["speed"];
    if (reticle.contains("maxX")) result.maxX = reticle["maxX"];
    if (reticle.contains("minX")) result.minX = reticle["minX"];
    if (reticle.contains("maxY")) result.maxY = reticle["maxY"];
    if (reticle.contains("minY")) result.minY = reticle["minY"];
    if (reticle.contains("forwardDistance")) result.forwardDistance = reticle["forwardDistance"];
    return result;
}

PlayerWeaponData PlayerDataLoader::LoadWeapon(const nlohmann::json& weapon) {
    PlayerWeaponData result{};
    if (weapon.contains("bulletInterval")) result.bulletInterval = weapon["bulletInterval"];
    if (weapon.contains("bulletSpeed")) result.bulletSpeed = weapon["bulletSpeed"];
    return result;
}

PlayerDeathData PlayerDataLoader::LoadDeath(const nlohmann::json& death) {
    PlayerDeathData result{};
    if (death.contains("duration")) result.duration = death["duration"];
    if (death.contains("fallSpeedY")) result.fallSpeedY = death["fallSpeedY"];
    if (death.contains("fallSpeedZ")) result.fallSpeedZ = death["fallSpeedZ"];
    if (death.contains("blinkSpeed")) result.blinkSpeed = death["blinkSpeed"];
    if (death.contains("blinkThreshold")) result.blinkThreshold = death["blinkThreshold"];
    if (death.contains("rotateSpeedX")) result.rotateSpeedX = death["rotateSpeedX"];
    if (death.contains("rotateSpeedZ")) result.rotateSpeedZ = death["rotateSpeedZ"];
    if (death.contains("shakeAmount")) result.shakeAmount = death["shakeAmount"];
    if (death.contains("shakeCycleX")) result.shakeAmount = death["shakeCycleX"];
    if (death.contains("shakeCyclez")) result.shakeAmount = death["shakeCyclez"];
    if (death.contains("shakeAmountMultZ")) result.shakeAmount = death["shakeAmountMultZ"];
    return result;
}