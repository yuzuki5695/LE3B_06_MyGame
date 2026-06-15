#pragma once
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Transform.h>
#include <ModelData.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <list>
#include <memory>

namespace MyEngine {

    // ゲーム全体の最大パーティクルグループ数の目安
    static const uint32_t MaxGroupCount = 64;

    // ゲーム全体で1フレームにスポーンできる合計数の器
    static const uint32_t MaxSpawnRequestCount = 4000;

    struct CameraData {
        Matrix4x4 view;
        Matrix4x4 projection;
        Matrix4x4 billboard;
    };

    struct ParticleInfo {
        uint32_t particleCount;
        float padding[3];
    };

    struct ParticleForGPU {
        Vector3 translate;          float pad0;
        Vector3 rotate;             float pad1;
        Vector3 scale;              float pad2;
        Vector4 color;
        Vector3 velocityTranslate;  float pad3;
        Vector3 velocityRotate;     float pad4;
        Vector3 velocityScale;      float pad5;
        float lifetime;
        float currentTime;
        uint32_t useGravity;        float pad6;
        uint32_t isAlive;

        float startAlpha;           float pad7[3];
    };

    struct SpawnRequestGPU {
        Vector3 translate;          float pad0;
        Vector3 rotate;             float pad1;
        Vector3 scale;              float pad2;
        Vector4 color;

        Vector3 velocityTranslate;  float pad3;
        Vector3 velocityRotate;     float pad4;
        Vector3 velocityScale;      float pad5;

        float lifetime;
        uint32_t useGravity;
        float padding;
    };

    struct GroupSpawnCB {
        uint32_t startRequestIndex;
        uint32_t spawnCount;
        float padding[62]; // 256バイト境界合わせ
    };

    template<typename T>
    struct RandomRange { T min{};    T max{};    bool enable = false; };
    struct ParticleSpawnRandom {
        RandomRange<Vector3> translate;
        RandomRange<Vector3> rotate;
        RandomRange<Vector3> scale;

        RandomRange<Vector4> color;

        RandomRange<Vector3> velocityTranslate;
        RandomRange<Vector3> velocityRotate;
        RandomRange<Vector3> velocityScale;

        RandomRange<float> lifetime;
        RandomRange<uint32_t> count;
    };

    // ─── CPU・システム内管理用構造体 ───

    struct Velocity {
        Vector3 translate;
        Vector3 rotate;
        Vector3 scale;
    };

    struct ParticleSpawnData {
        Transform transform;
        Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Velocity velocity{};
        float lifetime = 1.0f;
        uint32_t count = 1;
        bool useGravity = false;
    };

    struct SpawnRequest {
        std::string groupName;
        ParticleSpawnData spawnData;
    };

    struct ParticleGroup {
        std::unique_ptr<ParticleModel> model;                    // パーティクルモデル
        MaterialData materialData;                               // マテリアルデータ(テクスチャファイルパスとテクスチャ用SRVインデックス)	
        Microsoft::WRL::ComPtr <ID3D12Resource> Resource;        // インスタンシングリソース 
        Microsoft::WRL::ComPtr <ID3D12Resource> uploadResource;        // インスタンシングリソース 
        uint32_t maxInstanceCount = 0;  // 最大インスタンス数
        uint32_t srvindex;                                       // インスタンシング用SRVインデックス
        uint32_t uavIndex;                                       // インスタンシング用UAVインデックス

        D3D12_RESOURCE_STATES particleState = D3D12_RESOURCE_STATE_COMMON;
        // FreeList
        Microsoft::WRL::ComPtr<ID3D12Resource> freeListResource;
        Microsoft::WRL::ComPtr<ID3D12Resource> freeListUpload;
        D3D12_RESOURCE_STATES freeListState = D3D12_RESOURCE_STATE_COMMON;
        uint32_t freeListUavIndex = 0;
        // FreeCounter
        Microsoft::WRL::ComPtr<ID3D12Resource> freeCounterResource;
        Microsoft::WRL::ComPtr<ID3D12Resource> counterUpload;
        D3D12_RESOURCE_STATES freeCounterState = D3D12_RESOURCE_STATE_COMMON;
        uint32_t freeCounterUavIndex = 0;
    };
}