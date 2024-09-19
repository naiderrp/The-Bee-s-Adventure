struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct PassData
{
    float4x4 MVP;
};

ConstantBuffer<PassData> gPassData : register(b0);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(gPassData.MVP, float4(input.pos, 1.0f));
    output.uv = input.uv;
    return output;
}