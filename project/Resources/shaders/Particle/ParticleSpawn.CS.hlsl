#include "Particle.hlsli"

struct SpawnRequestGPU
{
    float3 translate;         float pad0;  // 16バイト
    float3 rotate;            float pad1;  // 16バイト
    float3 scale;             float pad2;  // 16バイト
    float4 color;                          // 16バイト
    float3 velocityTranslate; float pad3;  // 16バイト
    float3 velocityRotate;    float pad4;  // 16バイト
    float3 velocityScale;     float pad5;  // 16バイト
    
    // 最後の16バイトブロック
    float lifetime;                        // 4バイト
    uint useGravity;                       // 4バイト
    uint targetIndex;                      // 4バイト ★ここをC++と完全一致させる
    float padding;                         // 4バイト
};

RWStructuredBuffer<ParticleData> gParticle : register(u0);

// b0: 今回のグループの描画範囲
cbuffer GroupSpawnCB : register(b0)
{
    uint gStartRequestIndex;
    uint gSpawnCount;
};

// t0: 大元の全スポーン要求リスト (StructuredBuffer)
StructuredBuffer<SpawnRequestGPU> gSpawnRequests : register(t0);

// ★ スレッド数を 64 に増やし、一気に並列処理させます
[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 自分が今回の生成上限を超えていたら何もしない
    if (DTid.x >= gSpawnCount)
    {
        return;
    }

    // 全体配列から、自分のスレッドが担当するスポーン要求データを取得
    uint requestIndex = gStartRequestIndex + DTid.x;
    SpawnRequestGPU request = gSpawnRequests[requestIndex];

    // 要求データ内に仕込まれている、上書き対象のパーティクルインデックスを取得
    uint pIndex = request.targetIndex;

    // メインバッファへ一斉書き込み
    gParticle[pIndex].translate = request.translate;
    gParticle[pIndex].rotate = request.rotate;
    gParticle[pIndex].scale = request.scale;
    gParticle[pIndex].color = request.color;
    gParticle[pIndex].velocityTranslate = request.velocityTranslate;
    gParticle[pIndex].velocityRotate = request.velocityRotate;
    gParticle[pIndex].velocityScale = request.velocityScale;
    gParticle[pIndex].lifetime = request.lifetime;
    gParticle[pIndex].currentTime = 0.0f;
    gParticle[pIndex].useGravity = request.useGravity;
    gParticle[pIndex].startAlpha = request.color.a;
}