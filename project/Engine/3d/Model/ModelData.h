#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>

namespace MyEngine {
    /// <summary>
    /// 頂点データ
    /// </summary>
    struct VertexData final {
        Vector4 position{};
        Vector2 texcoord{};
        Vector3 normal{};
    };

    /// <summary>
    /// 定数バッファ用マテリアル (GPUに送るデータ)
    /// 16バイトアライメントを意識した構成
    /// </summary>
    struct Material final {
        Vector4 color;
        int32_t endbleLighting;
        float padding[3];
        Matrix4x4 uvTransform;
        float shinimess;
    };

    /// <summary>
    /// アセットとしてのマテリアルデータ
    /// </summary>
    struct MaterialData final {
        std::string textureFilePath;
        uint32_t textureindex = 0;
    };

    /// <summary>
    /// モデルデータ全体
    /// </summary>
    struct ModelData final {
        std::vector<VertexData> vertices;
        MaterialData material;
    };
}