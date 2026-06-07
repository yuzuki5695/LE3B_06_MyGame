#include "Particle.hlsli"

struct ParticleForGRU
{
    float3 translate;
    float pad0;

    float3 rotate;
    float pad1;

    float3 scale;
    float pad2;

    float4 color;
    
    
    // 追加
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

cbuffer CameraData : register(b1)
{
    float4x4 view;
    float4x4 projection;
    float4x4 billboard;
};

StructuredBuffer<ParticleForGRU> gParticle : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    ParticleForGRU particle = gParticle[instanceId];
    // ★ 寿命に達している、または未生成のものは完全に潰す（描画しない）
    if (particle.lifetime <= 0.0f || particle.currentTime >= particle.lifetime)
    {
        output.position = float4(0.0f, 0.0f, 0.0f, 0.0f);
        output.texcoord = float2(0.0f, 0.0f);
        output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        return output;
    }
        
    float4 pos = input.position;
    // Scale
    pos.xyz *= particle.scale;

    // Rotate ZXY
    float cx = cos(particle.rotate.x);
    float sx = sin(particle.rotate.x);

    float cy = cos(particle.rotate.y);
    float sy = sin(particle.rotate.y);

    float cz = cos(particle.rotate.z);
    float sz = sin(particle.rotate.z);

    // Z rotation
    float3 zRot;
    zRot.x = pos.x * cz - pos.y * sz;
    zRot.y = pos.x * sz + pos.y * cz;
    zRot.z = pos.z;

    // X rotation
    float3 xRot;
    xRot.x = zRot.x;
    xRot.y = zRot.y * cx - zRot.z * sx;
    xRot.z = zRot.y * sx + zRot.z * cx;

    // Y rotation
    float3 yRot;
    yRot.x = xRot.x * cy + xRot.z * sy;
    yRot.y = xRot.y;
    yRot.z = -xRot.x * sy + xRot.z * cy;

    pos.xyz = yRot;

    // Billboard
    pos = mul(pos, billboard);
    // Translate
    pos.xyz += particle.translate;

    // View Projection
    pos = mul(pos, view);
    pos = mul(pos, projection);

    // Output
    output.position = pos;
    output.texcoord = input.texcoord;
    output.color = particle.color;   
    return output;
}