
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
};

RWStructuredBuffer<ParticleData> gParticle : register(u0);

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

    // 寿命が 0 以下の未使用バッファ、またはすでに寿命が尽きているものは完全にスルー
    if (gParticle[index].lifetime <= 0.0f || gParticle[index].currentTime >= gParticle[index].lifetime)
    {
        // 念のため、未使用・寿命切れのものは描画されないようにスケールとアルファを完全に0にしておく
        gParticle[index].scale = float3(0.0f, 0.0f, 0.0f);
        gParticle[index].color.a = 0.0f;
        return;
    }
    
    // currentTime
    gParticle[index].currentTime += 1.0f / 60.0f;
    // alpha fade
    float alpha = 1.0f - gParticle[index].currentTime / gParticle[index].lifetime;
    gParticle[index].color.a = alpha;

    // 寿命に達したらデータを完全にゼロクリアして、次回VSで絶対に描画されないようにする
    if (gParticle[index].currentTime >= gParticle[index].lifetime)
    {
        gParticle[index].color.a = 0.0f;
        gParticle[index].scale = float3(0.0f, 0.0f, 0.0f);
        return;
    }
    // --- 以下、移動・重力計算（変更なし） ---
    if (gParticle[index].useGravity)
    {
        gParticle[index].velocityTranslate.y -= 0.003f;
    }
    gParticle[index].translate += gParticle[index].velocityTranslate;
    gParticle[index].rotate += gParticle[index].velocityRotate;
    gParticle[index].scale += gParticle[index].velocityScale;
}