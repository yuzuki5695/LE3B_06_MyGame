#include "Particle.hlsli"

cbuffer CameraData : register(b1)
{
    float4x4 view;
    float4x4 projection;
    float4x4 billboard;
};

StructuredBuffer<ParticleData> gParticle : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    ParticleData particle = gParticle[instanceId];
    // 死んでいるものは描画しない
    if (particle.isAlive == 0)
    {
        output.position = float4(0.0f, 0.0f, 0.0f, 0.0f);
        output.texcoord = float2(0.0f, 0.0f);
        output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        return output;
    }

    // 1. ローカル座標系での変形（Scale）
    float4 localPos = input.position;
    localPos.xyz *= particle.scale;

    // 2. ローカル座標系での回転（ZXY Rotation）
    float cx = cos(particle.rotate.x);
    float sx = sin(particle.rotate.x);
    float cy = cos(particle.rotate.y);
    float sy = sin(particle.rotate.y);
    float cz = cos(particle.rotate.z);
    float sz = sin(particle.rotate.z);

    // Z rotation
    float3 zRot;
    zRot.x = localPos.x * cz - localPos.y * sz;
    zRot.y = localPos.x * sz + localPos.y * cz;
    zRot.z = localPos.z;

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

    localPos.xyz = yRot;

    // 3. 【重要】移動する前にビルボードを適用（モデルの中心を中心にカメラを向く）
    // w成分を1.0に保つため、float4として行列乗算を行います
    float4 worldPos = mul(billboard, localPos);

    // 4. ワールド空間への配置（Translate）
    worldPos.xyz += particle.translate;

    // 5. ビュー・プロジェクション変換
    float4 finalPos = mul(worldPos, view);
    finalPos = mul(finalPos, projection);

    // 出力
    output.position = finalPos;
    output.texcoord = input.texcoord;
    output.color = particle.color;
    return output;
}