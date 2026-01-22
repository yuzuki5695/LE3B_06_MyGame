#pragma once

/// <summary>
/// リソースの種類を表す列挙体
/// </summary>
enum class AssetType {
    Texture,   // テクスチャ
    Model,     // 3Dモデル
    Audio,     // 音源
    Unknown    // 非対応拡張子
};