#include "Particle.hlsli"

RWStructuredBuffer<ParticleData> gParticle : register(u0);

// 空きインデックス管理
RWStructuredBuffer<uint> gFreeList : register(u1);
RWStructuredBuffer<int> gFreeCounter : register(u2);

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
    
    //-----------------------------------
    // FreeList から index を取得
    //-----------------------------------
    int freeCountBefore;
    InterlockedAdd(gFreeCounter[0], -1, freeCountBefore);
    if (freeCountBefore <= 0)
    {
        InterlockedAdd(gFreeCounter[0], 1);
        return;
    }
    // 空き index 取得
    uint pIndex = gFreeList[freeCountBefore - 1];
    //-----------------------------------
    // Particle 初期化
    //-----------------------------------  
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
    gParticle[pIndex].isAlive = 1;
    gParticle[pIndex].startAlpha = request.color.a;
    //-----------------------------
    // behavior 初期化
    //-----------------------------
    gParticle[pIndex].behaviorType = request.behaviorType;
    gParticle[pIndex].state = 0;
    gParticle[pIndex].stopTime = request.stopTime;
    gParticle[pIndex].targetPosition = float3(0, 0, 0);
}