#include "Particle.hlsli"

struct ParticleData
{
    float3 translate;
    float pad0;

    float3 rotate;
    float pad1;

    float3 scale;
    float pad2;

    float4 color;

    float3 velocityTranslate;
    float pad3;

    float3 velocityRotate;
    float pad4;

    float3 velocityScale;
    float pad5;

    float lifetime;
    float currentTime;
    uint useGravity;
    float pad6;

    float startAlpha;
    float pad7[3];
};

struct SpawnParticleGPU
{
    float3 translate;
    float pad0;

    float3 rotate;
    float pad1;

    float3 scale;
    float pad2;

    float4 color;

    float3 velocityTranslate;
    float pad3;

    float3 velocityRotate;
    float pad4;

    float3 velocityScale;
    float pad5;

    float lifetime;
    uint useGravity;
};

RWStructuredBuffer<ParticleData> gParticle
    : register(u0);

cbuffer SpawnInfo : register(b0)
{
    uint spawnIndex;
};

cbuffer SpawnParticle : register(b1)
{
    SpawnParticleGPU spawnParticle;
};

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    gParticle[spawnIndex].translate = spawnParticle.translate;
    gParticle[spawnIndex].rotate = spawnParticle.rotate;
    gParticle[spawnIndex].scale = spawnParticle.scale;

    gParticle[spawnIndex].color = spawnParticle.color;

    gParticle[spawnIndex].velocityTranslate = spawnParticle.velocityTranslate;
    gParticle[spawnIndex].velocityRotate = spawnParticle.velocityRotate;
    gParticle[spawnIndex].velocityScale = spawnParticle.velocityScale;

    gParticle[spawnIndex].lifetime = spawnParticle.lifetime;
    gParticle[spawnIndex].currentTime = 0.0f;
    gParticle[spawnIndex].useGravity = spawnParticle.useGravity;
    gParticle[spawnIndex].startAlpha = spawnParticle.color.a;
}