
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

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