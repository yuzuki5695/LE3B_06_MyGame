
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

struct SpawnRequestGPU
{
    float3 translate;
    float pad0; // 16バイト
    float3 rotate;
    float pad1; // 16バイト
    float3 scale;
    float pad2; // 16バイト
    
    float4 color; // 16バイト
 
    float3 velocityTranslate;
    float pad3; // 16バイト
    float3 velocityRotate;
    float pad4; // 16バイト
    float3 velocityScale;
    float pad5; // 16バイト
    
    // 最後の16バイトブロック
    float lifetime; // 4バイト
    uint useGravity; // 4バイト
    uint targetIndex; // 4バイト ★ここをC++と完全一致させる
    float padding; // 4バイト
};