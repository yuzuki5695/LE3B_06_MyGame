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
cbuffer AttractInfo : register(b1)
{
    float3 gTargetPosition;
    float gDeltaTime;
};
void UpdateNormal(uint index)
{
    gParticle[index].translate += gParticle[index].velocityTranslate;
    gParticle[index].rotate += gParticle[index].velocityRotate;
    gParticle[index].scale += gParticle[index].velocityScale;
}

void UpdateExplosion(uint index)
{
    gParticle[index].translate += gParticle[index].velocityTranslate;
    gParticle[index].rotate += gParticle[index].velocityRotate;
    gParticle[index].scale += gParticle[index].velocityScale;
}
void UpdateAttract(uint index)
{
    //--------------------------------
    // state0 : 爆散
    //--------------------------------
    if (gParticle[index].state == 0)
    {
        gParticle[index].translate += gParticle[index].velocityTranslate;
        gParticle[index].rotate += gParticle[index].velocityRotate;
        gParticle[index].scale += gParticle[index].velocityScale;

        // stopTime経過
        if (gParticle[index].currentTime >= gParticle[index].stopTime)
        {
            gParticle[index].state = 1;

            // 停止
            gParticle[index].velocityTranslate = float3(0, 0, 0);
            gParticle[index].velocityRotate = float3(0, 0, 0);
            gParticle[index].velocityScale = float3(0, 0, 0);
        }

        return;
    }

    //--------------------------------
    // state1 : 一瞬停止
    //--------------------------------
    if (gParticle[index].state == 1)
    {
        // 即吸引開始
        gParticle[index].state = 2;
        return;
    }

    //--------------------------------
    // state2 : プレイヤー吸引
    //--------------------------------
    if (gParticle[index].state == 2)
    {
        float3 toTarget = gTargetPosition - gParticle[index].translate;
        float dist = length(toTarget);

        float3 dir = normalize(toTarget);
        // 遠いほど速い
        float speed = lerp(1.0f, 8.0f, saturate(dist / 10.0f));
        // 目標速度へ補間
        gParticle[index].velocityTranslate = lerp(gParticle[index].velocityTranslate, dir * speed, 0.12f);
        // 移動
        gParticle[index].translate += gParticle[index].velocityTranslate;
        // プレイヤーに近づいたら消す
        if (dist < 0.5f)
        {
            gParticle[index].isAlive = 0;
            gParticle[index].color.a = 0.0f;
            gParticle[index].scale = float3(0, 0, 0);

            int freeIndex;
            InterlockedAdd(gFreeCounter[0], 1, freeIndex);
            gFreeList[freeIndex] = index;
            return;
        }
    }
}

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
    switch (gParticle[index].behaviorType)
    {
        case 0:
            UpdateNormal(index);
            break;

        case 1:
            UpdateExplosion(index);
            break;

        case 2:
            UpdateAttract(index);
            break;
    }
}