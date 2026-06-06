
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
       
    //----------------------------------
    // 範囲外アクセス防止
    //----------------------------------
    if (index >= particleCount)
    {
        return;
    }
    
    //----------------------------------
    // currentTime
    //----------------------------------
    gParticle[index].currentTime += 1.0f / 60.0f;

    //----------------------------------
    // lifetime check
    //----------------------------------
    if (gParticle[index].currentTime >= gParticle[index].lifetime)
    {
        gParticle[index].color.a = 0.0f;
        return;
    }

    //----------------------------------
    // alpha fade
    //----------------------------------
    float alpha = 1.0f - gParticle[index].currentTime / gParticle[index].lifetime;

    gParticle[index].color.a = alpha;

    //----------------------------------
    // gravity
    //----------------------------------
    if (gParticle[index].useGravity)
    {
        gParticle[index].velocityTranslate.y -= 0.003f;
    }

    //----------------------------------
    // translate
    //----------------------------------
    gParticle[index].translate += gParticle[index].velocityTranslate;

    //----------------------------------
    // rotate
    //----------------------------------
    gParticle[index].rotate += gParticle[index].velocityRotate;

    //----------------------------------
    // scale
    //----------------------------------
    gParticle[index].scale += gParticle[index].velocityScale;
}