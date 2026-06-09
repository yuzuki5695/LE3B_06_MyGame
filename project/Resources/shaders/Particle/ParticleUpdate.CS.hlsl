#include "Particle.hlsli"

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
    
    // 💡 既に完全に死亡していて、スケールもクリア済みのものは「何もしないで即座に抜ける」
    // これにより、UAVへの無駄なメモリ書き込み（Store操作）がゼロになり、GPUの電力が大幅に下がります
    if (gParticle[index].lifetime <= 0.0f || (gParticle[index].currentTime >= gParticle[index].lifetime && gParticle[index].scale.x == 0.0f))
    {
        return;
    }
    
    // currentTime
    gParticle[index].currentTime += 1.0f / 60.0f;
    // 生存率(1→0)
    float lifeRate = 1.0f - gParticle[index].currentTime / gParticle[index].lifetime;
    // 初期アルファを維持しながらフェード
    gParticle[index].color.a = gParticle[index].startAlpha * lifeRate;
    
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
        //gParticle[index].velocityTranslate.y -= 0.003f;
    }
    gParticle[index].translate += gParticle[index].velocityTranslate;
    gParticle[index].rotate += gParticle[index].velocityRotate;
    gParticle[index].scale += gParticle[index].velocityScale;
}