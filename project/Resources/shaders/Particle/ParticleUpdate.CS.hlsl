#include "Particle.hlsli"

RWStructuredBuffer<ParticleData> gParticle : register(u0);
// FreeList
RWStructuredBuffer<uint> gFreeList : register(u1);
RWStructuredBuffer<uint> gFreeCounter : register(u2);

cbuffer ParticleInfo : register(b0)
{
    uint particleCount;
    float3 padding;
};

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    if (index >= particleCount)
        return; 
    
    //---------------------------------
    // 死亡済みなら何もしない
    //---------------------------------
    if (!gParticle[index].isAlive)
    {
        return;
    }
    //---------------------------------
    // 時間更新
    //---------------------------------
    // currentTime
    gParticle[index].currentTime += 1.0f / 60.0f;
    // 生存率(1→0)
    float lifeRate = 1.0f - gParticle[index].currentTime / gParticle[index].lifetime;
    // 初期アルファを維持しながらフェード
    gParticle[index].color.a = gParticle[index].startAlpha * lifeRate;
    //---------------------------------
    // 死亡
    //---------------------------------
    if (gParticle[index].currentTime >= gParticle[index].lifetime)
    {
        // 死亡フラグ
        gParticle[index].isAlive = 0;
        // 描画防止
        gParticle[index].color.a = 0.0f;
        gParticle[index].scale = float3(0, 0, 0);
        //---------------------------------
        // FreeListへ返却
        //---------------------------------
        int freeIndex;
        InterlockedAdd(gFreeCounter[0], 1, freeIndex);
        gFreeList[freeIndex] = index;
        return;
    }

    //---------------------------------
    // 更新
    //---------------------------------
    if (gParticle[index].useGravity)
    {
        //gParticle[index].velocityTranslate.y -= 0.003f;
    }
    gParticle[index].translate += gParticle[index].velocityTranslate;
    gParticle[index].rotate += gParticle[index].velocityRotate;
    gParticle[index].scale += gParticle[index].velocityScale;
}