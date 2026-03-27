#pragma once
#include <string>
#include <fstream>
#include <json.hpp>
#include <Vector3.h>
#include <Transform.h>

/// <summary>
/// キャラクター用JSONローダーの共通基底クラス
/// 共通で使う「読み込み処理」をまとめたクラス
/// </summary>
class BaseCharacterJsonLoader {
protected: // メンバ関数

    /// <summary>
    /// JSON配列 → Vector3 へ変換
    /// [x, y, z] の形式をそのままVector3に変換
    /// </summary>
    /// <param name="j">JSON配列</param>
    /// <returns>Vector3</returns>
    static Vector3 ParseVector3(const nlohmann::json& vector3) {
        return Vector3{ (float)vector3[0],(float)vector3[1],(float)vector3[2] };
    }

    /// <summary>
    /// Transformの共通読み込み処理
    /// translation / rotation / scaling をJSONから取得する
    /// </summary>
    /// <param name="transform"></param>
    /// <returns>Transform</returns>
    static Transform LoadTransform(const nlohmann::json& transform) {
        Transform result{};

        // 座標
        if (transform.contains("translation")) {
            result.translate = ParseVector3(transform["translation"]);
        }

        // 回転
        if (transform.contains("rotation")) {
            Vector3 rot = ParseVector3(transform["rotation"]);
            // 必要に応じて符号反転
            result.rotate = { -rot.x, -rot.y, -rot.z };
        }

        // サイズ
        if (transform.contains("scaling")) {
            result.scale = ParseVector3(transform["scaling"]);
        }

        return result;
    }

    /// <summary>
    /// JSONファイルを開いて読み込む共通処理
    /// </summary>
    /// <param name="fullPath">フルパス</param>
    /// <returns>JSONデータ</returns>
    static nlohmann::json OpenFile(const std::string& fullPath) {
        std::ifstream file(fullPath);

        // ファイルが開けなかった場合は例外
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON: " + fullPath);
        }

        nlohmann::json j;
        file >> j; // JSONとして読み込み

        return j;
    }
};